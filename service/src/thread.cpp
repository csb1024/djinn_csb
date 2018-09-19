
/*
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/**
 * @author: Johann Hauswald, Yiping Kang
 * @contact: jahausw@umich.edu, ypkang@umich.edu
 */
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <stdint.h>
#include <ctime>
#include <cmath>
#include <glog/logging.h>
#include <boost/chrono/thread_clock.hpp>
#include <time.h>
#include <cstring>
#include "timer.h"
#include "thread.h"
//#include "ts_queue.h"
#include "ts_list.h"
#include "request.h"
#include "batched_request.h"
#include "scheduler.h"

extern GlobalScheduler gsc;
extern bool debug;
extern bool gpu;
extern unsigned int task_id;
extern string net_list;
extern string common;
extern string weight_dir;
extern int gpu_turn;
extern FILE* log_file;
extern int CPUID;
extern exec_mode gexec_mode;
extern exec_mode cexec_mode;

vector<map<string, Net<float>*>> nets;
map<string,TSList<request>*> rlist_hash;
map<string, mutex*> rmtx_vec;
mutex cnt_mtx;
mutex gsc_mtx;
map<string, int> batch_table;


static uint64_t getCurNs() {
	   struct timespec ts; 
	      clock_gettime(CLOCK_REALTIME, &ts);
	         uint64_t t = ts.tv_sec*1000*1000*1000 + ts.tv_nsec;
		    return t;
}
mutex cublas_mtx; // used for dealing issues with Cublas API calls


vector<map<string, bool>*> isRunningVec; // vector of pointers that point per GPU maintained table, tracking whether request is running on the GPU
map<int, exec_mode> device_exec_mode;


//from a new and empty batch for a given task and device number 
void formEmptyBatch(batched_request &in, string net_name, int device_id){
	int maxbatch;
	in.setReqName(net_name.c_str());
	in.setState(EMPTY);
	// 1 set the task's device which it will run on
	in.setGPUID(device_id);
	// 2 decide the maximum batch size allowed 
	if (device_exec_mode[device_id] == CON || device_exec_mode[device_id]==B2B)
			maxbatch=1;
			else if(device_exec_mode[device_id] == PEB) {
				if (!strcmp(in.getReqName(),"dig" ))
					maxbatch=32;
				else
					maxbatch = 2^31-1; 
				}						
			else{
				if (device_id == CPUID )
					maxbatch = gsc.getMaxBatch(net_name,"cpu");
				else
					maxbatch = gsc.getMaxBatch(net_name,"gpu");							
			}
		in.setMaxBatch(maxbatch);
	return;
}


//send results to each client 
void sendBatchedResults(batched_request *brp, float *out, int out_elts){
	int nclient = brp-> getNTask();
	string str_name(brp->taskRequests[0]->getReqName());
//	int offset=0; //used for indexing the start and the end of the output for each client
//	float *temp;
//	bool local_debug =1;
#ifdef DEBUG
//	printf("sending back results of %d clients back \n",nclient);
#endif
	rmtx_vec[str_name]->lock();
	for(int i =0; i<nclient; i++)
	{
		request* rtemp = brp->taskRequests[i];		
/*		int partial_out_elts = (int)(out_elts * ((float)rtemp->getDataLen()/brp->getDataSize()));
		temp=(float *)malloc(sizeof(float)*partial_out_elts);
		memcpy(temp,out+offset,sizeof(float)*partial_out_elts);
		offset+=partial_out_elts;
		SOCKET_send(rtemp->getClientFD(), (char*)temp, partial_out_elts * sizeof(float), debug);*/
		rtemp->setState(END);
//		free(temp);
//		close(rtemp->getClientFD());
	}
	rmtx_vec[str_name]->unlock();
}
 

pthread_t request_thread_init(int sock){
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 1024 * 1024); // set memory size, may need to adjust in the future, for now set it to 1MB
  pthread_t tid;
  if (pthread_create(&tid, &attr, request_handler, (void *)(intptr_t)sock) != 0)
    LOG(ERROR) << "Failed to create a request handler thread.\n";
  return tid;

}
//// request handler ////

//request handler : sends request to request list
void* request_handler(void* sock){
 uint64_t start;
 
 int socknum = (intptr_t)sock;
 char net_name[MAX_REQ_SIZE]; 
 int local_debug = 1;    
//receive request 
SOCKET_receive(socknum, (char*)&net_name, MAX_REQ_SIZE, debug); 
string str_name(net_name);
start=getCurNs();
//need to check valid request name
  map<string, Net<float>*>::iterator it = nets[0].find(net_name);
  if (it == nets[0].end()) {
   LOG(ERROR) << "Task " << net_name << " not found.";
    return (void*)1;
  } else
    LOG(INFO) << "Task " << net_name << " forward pass.";
//receive batch size
int batch_size = SOCKET_rxsize(socknum); 

//receive data length : number of data x data size
int data_len = SOCKET_rxsize(socknum);

printf("received data! batch_size : %d, data_len: %d \n",batch_size,data_len);

 while (1) {   

float *in_data = (float *)malloc(data_len * sizeof(float));
   int rcvd = SOCKET_receive(socknum, (char *)in_data,data_len * sizeof(float) ,debug);

 if (rcvd == 0) break;  // Client closed the socket    

//make a request and push it to the list
cnt_mtx.lock();
task_id++;
cnt_mtx.unlock();

request *tempRequest = new request(task_id, socknum, batch_size);
tempRequest->setState(QUEUED);
tempRequest->setReqName(net_name);
//update the batch table
tempRequest->_input.assign(in_data,in_data+data_len);
rmtx_vec[str_name]->lock();
rlist_hash[str_name]->pushBack(*tempRequest);
rmtx_vec[str_name]->unlock();
batch_table[str_name]++;
#ifdef DEBUG
printf(" Made request task id  %d which is  %u bytes long \n", (task_id-1),tempRequest->_input.size());
#endif 
tempRequest->end_recv=getCurNs();
tempRequest->start=start;
}
close(socknum);
}

//// check list and call exeuction handler for batch request ////
/// 

void* batch_handler(void *args){
  int master_id=(intptr_t)args; // the device id to work for... 
batched_request* btemp=NULL; // the local batched_request;

  // we will  batched requests per tasks locally
  map<string, batched_request*> br_table; 

     for (map<string, int>::iterator it = batch_table.begin(); it != batch_table.end(); it++){ br_table[it->first]=NULL;
     }

  
  while (1){
//   if (!rlist.getLength()) continue; // skip if list is enpty 
  int j =0;
   //check for existing requests , I know I can just cache the information whether this thread is responsible or devices can be picked dynaically in the future
     for (map<string, int>::iterator it = batch_table.begin(); it != batch_table.end(); it++){
	if (it->second){
		//check if this thread is responsible for batching the request
		int device_id;
		if (gpu){
			gsc_mtx.lock();
			device_id = gsc.executeScheduler(it->first.c_str(),0); //deciding size of batch must change in the future...maybe the batch table can keep the number of requests?
			gsc_mtx.unlock();
		}
		else device_id=CPUID;
		if (device_id != master_id) // if this thread is not responsible, then skip it
			continue;
	//check whether a batch is pending or not
	string str_net_name=it->first;
	int maxbatch;
	btemp=  br_table[it->first];
	if (!(br_table[it->first])){ // if btemp is null
	br_table[it->first] = new batched_request(); // IMPORTANT: you have to intialize with the table directly not by a local varaiable e.g.) btemp
	btemp=  br_table[it->first];
	btemp->setReqName(str_net_name.c_str());
	btemp->setState(EMPTY);
	// 1 set the task's device which it will run on
	btemp->setGPUID(device_id);
	// 2 decide the maximum batch size allowed 
	if (device_exec_mode[device_id] == CON || device_exec_mode[device_id]==B2B)
			maxbatch=1;
			else if(device_exec_mode[device_id] == PEB) {
				if (!strcmp(btemp->getReqName(),"dig" ))
					maxbatch=32;
				else
					maxbatch = 2^31-1; 
				}						
			else{
				if (device_id == CPUID )
					maxbatch = gsc.getMaxBatch(str_net_name,"cpu");
				else
					maxbatch = gsc.getMaxBatch(str_net_name,"gpu");							
			}
		btemp->setMaxBatch(maxbatch);
#ifdef DEBUG
		printf("[BATCH] make new batched request for %s on device %d \n", btemp->getReqName(), device_id);
#endif		
		}
		// find (listd ) request if there is one and add the data to it
//		printf("batched request's state : %d \n", btemp->getState());
		if (btemp->getState() == RUNNING) continue;
		if (btemp->getState() == END){
			br_table[it->first]->clearBatchInfo();
			//btemp = NULL;
			br_table[it->first]->setState(EMPTY);
			continue;

		}
		if (btemp->getState() == EMPTY || btemp->getState() == QUEUED){

			request* r=NULL;
			TSList<request>* temp_rlist=rlist_hash[str_net_name];
#ifdef DEBUG
//			printf("[BATCH] Request Length : %u  \n", temp_rlist->getLength());
#endif
			for (int i=0; i<temp_rlist->getLength(); i++)
			{ // look for listed requests
				r = temp_rlist->at(i);
				if (!r) continue;
//				printf("request state : %d \n ",r->getState());

				if (!(strcmp(r->getReqName(), btemp->getReqName())) && r->getState() == QUEUED ){//found ya!
						//check whether new request can be further batched											
					if (btemp->getNTask() < btemp->getMaxBatch()){					
						rmtx_vec[str_net_name]->lock();
						btemp->addRequestInfo(r);	
						btemp->setState(QUEUED);

						r->end_batch=getCurNs();
						r->setState(BATCHED);
						rmtx_vec[str_net_name]->unlock();						

#ifdef DEBUG
printf("[BATCH] found request %s ! now batching task id : %d \n",r->getReqName(), r->getTaskID());
printf("[BATCH] there are total %d %s tasks batched for %dth batched task \n  ",btemp->getNTask(), btemp->getReqName(), j);
#endif						
					}// if there are less tasks than max batch
						else break; // if control has jumped to here, it means batch is full. so break the loop
				} //found queued request								
			} //search temp_list for the request		
	
		//START Execution according to device's scheduling policy
	if (  btemp->getState() == QUEUED ){
	switch(device_exec_mode[master_id]){
		case CON:
			break; // NOP
		case B2B:
		case PEB:
		case EB:
			if (isRunningVec[master_id]->operator[](btemp->getReqName())) continue;
			isRunningVec[master_id]->operator[](btemp->getReqName())=true;
			break;

		default: // not suppose to happen
			printf("please check execution mode!");
			exit(1);
	}
	btemp->setState(RUNNING);
		#ifdef DEBUG
		if (device_id != CPUID)
		printf("[BATCH] found %s as batched request for gpu %d \n",btemp->getReqName(), device_id);
		else
		printf("[BATCH] found %s as batched request for cpu\n",btemp->getReqName());
#endif
		execution_thread_init(btemp);
	}
	}
	else if (btemp->getState() == EMPTY) // empty batched request, might happend due to table race or other batch has already batched everything
		continue;
		} //if such request exists in table    	
     	}  //check the table for existing requests
	}// infinite loop			 
}//batch handler function
  
pthread_t batching_thread_init(int gpu_id){
  int gpuID = gpu_id;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 1024 * 1024); // set memory size, may need to adjust in the future for now set to max * 1MB

  gpu_turn=0;
  pthread_t tid;
  if (pthread_create(&tid, &attr, batch_handler, (void *)(intptr_t)gpuID) != 0)
    LOG(ERROR) << "Failed to create a batching thread.\n";
  return tid;

}

//// batch execution handler ////
/// producer and consumer ///

void SERVICE_fwd(float* in, int in_size, float* out, int out_size,
                 Net<float>* net) {
#ifdef DEBUG
  string net_name = net->name();
  STATS_INIT("service", "DjiNN service inference");
  PRINT_STAT_STRING("network", net_name.c_str());
  
   if (Caffe::mode() == Caffe::CPU)
    PRINT_STAT_STRING("platform", "cpu");
  else
    PRINT_STAT_STRING("platform", "gpu");
 tic();
#endif
   //LOG(ERROR) << "got input blobs from net";
 float loss;
  vector<Blob<float>*> in_blobs = net->input_blobs();

  
   in_blobs[0]->set_cpu_data(in);
  vector<Blob<float>*> out_blobs;
  if (Caffe::mode() == Caffe::CPU){
	out_blobs = net->ForwardPrefilled(&loss);
	memcpy(out, out_blobs[0]->cpu_data(), sizeof(float));
  }

  else{
 	out_blobs = net->ForwardPrefilled(&loss);
	memcpy(out, out_blobs[0]->cpu_data(), sizeof(float));	
  }
 #ifdef DEBUG
  PRINT_STAT_DOUBLE("inference latency", toc());
  STATS_END();
#endif

  if (out_size != out_blobs[0]->count())
    LOG(FATAL) << "out_size =! out_blobs[0]->count())";
  else
    memcpy(out, out_blobs[0]->cpu_data(), out_size * sizeof(float));
}


pthread_t execution_thread_init(batched_request *req) {
  // Prepare to create a new pthread
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 1024 * 1024);
  
   // Create a new thread starting with the function execution_handler
#ifdef DEBUG 
  	printf("[EXEC] Initiate thread for %d tasks of %s \n",req->getNTask(), req->getReqName());
#endif 
  pthread_t tid;
  if (pthread_create(&tid, &attr, execution_handler, (void *)req) != 0)
    LOG(ERROR) << "Failed to create a execution handler thread.\n";

  return tid;
}

void* execution_handler(void* args) {
  batched_request *input_info = (batched_request *)args;
 for(int i=0; i <input_info->getNTask(); i++){
	  input_info->taskRequests[i]->prep_exec=getCurNs();
  }

  int gpu_id = input_info->getGPUID();

  char req_name[MAX_REQ_SIZE];
  strcpy(req_name, input_info->getReqName());


  uint64_t start,end;

  // 1. Client sends the application type
  // 2. Client sends the size of incoming data
  // 3. Client sends data
  map<string, Net<float>*>::iterator it = nets[0].find(req_name);
  if (it == nets[0].end()) {
    LOG(ERROR) << "Task " << req_name << " not found.";
    return (void*)1;
  } else
    LOG(INFO) << "Task " << req_name << " forward pass.";

  // receive the input data length (in float)
  int sock_elts = input_info->getDataSize();
 // int sock_elts = input_info->datalen;

  if (sock_elts < 0) {
    LOG(ERROR) << "Error num incoming elts.";
    return (void*)1;
  }

  // reshape input dims if incoming data != current net config
//  LOG(ERROR) << "Elements received for processing " << sock_elts << endl;
  reshape(nets[gpu_id][req_name], sock_elts);

  int in_elts = nets[gpu_id][req_name]->input_blobs()[0]->count();
  int out_elts = nets[gpu_id][req_name]->output_blobs()[0]->count();
  float* in = (float*)malloc(in_elts * sizeof(float));
  float* out = (float*)malloc(out_elts * sizeof(float));

   // Warmup: used to move the network to the device for the first time
  // In all subsequent forward passes, the trained model resides on the
// i dont know whether the next line is necessary... 
  // however code does not work if next line is crossed out
/*
    bool warmup = false;

    if (warmup && gpu) {
   printf("exeucting warmup\n");
      float loss;
      vector<Blob<float>*> in_blobs = nets[gpu_id][req_name]->input_blobs();
      in_blobs[0]->set_cpu_data(input_info->b_input.data());
      vector<Blob<float>*> out_blobs;
      out_blobs = nets[gpu_id][req_name]->ForwardPrefilled(&loss);
      warmup = false;
    }
*/
//   LOG(INFO) << "Executing forward pass.";
int tnum = input_info->getNTask();

#ifdef DEBUG
//    printf("total input size : %d \n", in_elts);
     printf("[EXEC]there are total %d tasks batched \n",tnum);
      printf("[EXEC]batched task id : ");
    for (int id=0; id<tnum;id++){
	    printf("%d ",input_info->taskRequests[id]->getTaskID());
    }
    printf("\n");
  //  printf("total %d elements are to be processed \n",sock_elts);
//   double start_e, end_e;
//   start_e = getCurNs();
#endif 
    if (gpu_id != CPUID){ // if GPU
	cublas_mtx.lock();
	Caffe::SetDevice(gpu_id);
	Caffe::set_mode(Caffe::GPU);

	 }
 else {// if CPU
	 Caffe::set_mode(Caffe::CPU);

 }
   for (int id=0; id<tnum;id++){
    	    input_info->taskRequests[id]->start_exec=getCurNs();
    }

//    for (int i =0; i<100000;i++){
      	SERVICE_fwd(input_info->b_input.data(), in_elts, out, out_elts, nets[gpu_id][req_name]);
   //  }
 if (gpu_id != CPUID) cublas_mtx.unlock();
     for (int id=0; id<tnum;id++){
    	    input_info->taskRequests[id]->end_exec=getCurNs();
}	
   isRunningVec[gpu_id]->operator[](req_name)=false;
//    LOG(ERROR) << "Writing to socket.";
    sendBatchedResults(input_info, out, out_elts);  
 
  free(in);
  free(out);
  input_info->setState(END);
  pthread_exit(NULL);
  return (void*)0;
}

pthread_t server_thread_init(int gpu_id){
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 8*1024 * 1024); // set memory size, may need to adjust in the future, for now set it to 1MB
  pthread_t tid;
  if (pthread_create(&tid, &attr, server_init, (void *)(intptr_t)gpu_id) != 0)
    LOG(ERROR) << "Failed to create a request handler thread.\n";
  return tid;
}

void* server_init(void*  gpu_id){

int gpuID = (intptr_t)gpu_id;
batching_thread_init(gpuID);
map<string, Net<float>*> tlnets;
map<string, bool> local_running; //table for checking whether request is running on the GPU
  if (gpuID == CPUID){//init cpu server

	Caffe::set_mode(Caffe::CPU);
	device_exec_mode[gpuID]=cexec_mode;
	}
  else if(gpu && gpuID != CPUID){  
	Caffe::SetDevice(gpuID);
	Caffe::set_mode(Caffe::GPU);
	device_exec_mode[gpuID]=gexec_mode;
  }
  Caffe::set_phase(Caffe::TEST);

 ifstream file(net_list.c_str());
 string net_name;

while (file >> net_name){
if (gpuID != CPUID)
	cout << "loading model " << net_name << " for gpu " << gpuID <<endl;
else
	cout << "loading model " << net_name << "on CPU DRAM" <<endl;

// init net hashes 
//first check whether is a list for the hash
map<string,TSList<request>*>::iterator it;
string pure_name = net_name.substr(0,net_name.size()-9);
it = rlist_hash.find(pure_name);
if (it == rlist_hash.end()){ // if not found make a new list and insert it
#ifdef DEBUG 
	printf("inserting %s into hash table(s)\n", pure_name.c_str());
#endif
	TSList<request> *rinput = new TSList<request>();
	rlist_hash[pure_name]=rinput;
	mutex *rmtx = new mutex();
	rmtx_vec[pure_name] = rmtx;
}

string net = common + "/configs/" + net_name;

Net<float>* temp = new Net<float>(net);
const std::string name = temp->name();
tlnets[name] = temp;
    std::string weights = common +
                          weight_dir + name + ".caffemodel";
    tlnets[name]->CopyTrainedLayersFrom(weights);
    batch_table[name]=false;
    //init the table
    local_running[name]=false; 
   cout << "preloaded "<<name<<endl;
}

// setup shared variables that will be referenced by other threads
nets.push_back(tlnets);
//initiate execution thread
isRunningVec.push_back(&local_running);
while (1){
// checking batch list within loop
} // infinite loop
}
pthread_t clear_thread_init(){
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 1024 * 1024); // set memory size, may need to adjust in the future, for now set it to 1MB
//init batch index table
  pthread_t tid;
  if (pthread_create(&tid, &attr, clear_handler, NULL) != 0)
    LOG(ERROR) << "Failed to create a request handler thread.\n";
  return tid;
}

//clears memory allocated for batched reqeusts and requests
void* clear_handler(){
	//check batched reqeust list for ended execution
	int cnt = 500; //used for adjusting the frequency of dbugging messages 
	while (1)
	{
		request* rtemp;
		for (map<string, int>::iterator it = batch_table.begin(); it != batch_table.end();it++ ){
//		rmtx_vec[it->first].lock();
		int i=0;
		if (rlist_hash[it->first]->getLength() == 0) continue;
		while (i<rlist_hash[it->first]->getLength()){
		rtemp = rlist_hash[it->first]->at(i); 
		if (!rtemp) break ; // if not found due to race, high probability that it is busy
		if (rtemp->getState() == END) {
		rtemp->writeToLog(log_file);
//		rmtx_vec[it->first]->lock();
		rlist_hash[it->first]->eraseElement(i);	
		batch_table[it->first]--;
//		rmtx_vec[it->first]->unlock();			
		#ifdef DEBUG
			printf("[CLEAR] cleaning up %d th request!\n",rtemp->getTaskID());		
		#endif
		}
		else
			i++;
		} // iterate through rlist
#ifdef DEBUG
		if (cnt == 0 ){
		printf("[CLEAR] total %d requests remaining for task %s \n",it->second,it->first.c_str());
		cnt = 500;
		}
		else cnt--;
#endif 
//		rmtx_vec[it->first].unlock();			
		} // iterate through the batch table(just for the network's name)
	} // infinite loop
}
/*
pthread_t batch_table_thread_init(){
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 1024 * 1024); // set memory size, may need to adjust in the future, for now set it to 1MB
//init batch index table
   for (map<string, int>::iterator it = batch_table.begin(); it != batch_table.end(); it++)
	it->second = 0;

  pthread_t tid;
  if (pthread_create(&tid, &attr, btable_handler, NULL) != 0)
    LOG(ERROR) << "Failed to create a request handler thread.\n";
  return tid;
}
void *btable_handler(){
	while(1){
		 for (map<string, int>::iterator it = batch_table.begin(); it != batch_table.end(); it++){
		if (rlist_hash[it->first]->getLength()==0)
			it->second = false;
		else
			it->second = true;
		 }
	}
}
*/
