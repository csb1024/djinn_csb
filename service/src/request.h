#ifndef __REQUEST
#define __REQUEST
#include <queue>
#include <deque>
#include <vector>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;

enum Task_state {EMPTY=0,QUEUED=1,FULL=2,BATCHED=3,RUNNING=4,END=5}; //BATCHED  is only used for 'requests'(not batched request), 
// EMPTY and FULLL is only used for batched_request

class request{
private:
  int taskID;
  int client_fd;
  int batch_num; // batch size
  Task_state state;
  char _req_name[MAX_REQ_SIZE];

  // the data itself , maybe in the future change this to FP16or a versatile pointer

public:
vector<float> _input; 
  request(int tid, int fd, int bnum)
 {
  taskID = tid;
  client_fd= fd;
  batch_num = bnum;
 }

  ~request(){} 

void setState(Task_state s){
	state = s;
}


Task_state getState(){
return state;
}

void setReqName(char * net_name){
strcpy(_req_name, net_name);
}

char* getReqName(){
return _req_name; 
}
vector<float>* getInternalInput(){
return &(_input);
}
 
int getTaskID(){
return taskID;

}

int getClientFD(){
return client_fd;
}

int getBatchNum(){
return batch_num;
}
int getDataLen(){
return _input.size();
}

 //for checking request's overhead
  uint64_t start;
  uint64_t end_recv;
  uint64_t end_batch;
  uint64_t prep_exec;
  uint64_t start_exec;
  uint64_t end_exec; 


void writeToLog(FILE* fp){
	double recv_time;
	double exec_time;
	double batch_time;
	double prep_time;
	double server_overhead;
	recv_time = double(end_recv-start);
	exec_time = double(end_exec - start_exec);
	batch_time = double(end_batch-end_recv);
	prep_time = double(start_exec - prep_exec);
	server_overhead	= double(end_exec - start) - (recv_time + exec_time + batch_time+prep_time);
	recv_time = recv_time / 1000000;
	exec_time = exec_time / 1000000;
	batch_time = batch_time / 1000000;
	prep_time = prep_time / 1000000;
	server_overhead = server_overhead / 1000000;
	fprintf(fp,"%s,%d,%lf,%lf,%lf,%lf,%lf\n",_req_name,taskID,recv_time,batch_time,exec_time,prep_time,server_overhead);
	fflush(fp);
	return;
}


   
};
#endif 
