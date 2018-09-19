#ifndef __BREQUEST
#define __BREQUEST
#include <queue>
#include <deque>
#include <vector>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "request.h"

using namespace std;

class batched_request{
private:
  int batch_num; // batch size
  int gpuid;// gpuid to execute
  Task_state state;
  char _req_name[MAX_REQ_SIZE];
  // the data itself , maybe in the future change this to FP16or a versatile pointer

  int _max_batch; //maximum number of tasks that can be  batched 


public:
vector<float> b_input;
vector<request*> taskRequests;
  batched_request() //makes empty batched_request
 {
  batch_num=0;
  gpuid=-1;
 }
  batched_request(request *r) // makes batched_request based on request
 {
	 taskRequests.push_back(r);
	 batch_num = r->getBatchNum();
//	 strcpy(_req_name,r->getReqName());
	b_input = r->_input;
	 gpuid=0;
 }

  ~batched_request(){} 

void setState(Task_state s){
	state = s;
}

vector<request*> getRequests(){
return taskRequests;
}

Task_state getState(){
return state;
}

char* getReqName(){
return _req_name; 
}

void setReqName(const char *net_name){
strcpy(_req_name, net_name);
}

int getBatchNum(){
return batch_num;
}


int getDataSize(){
return b_input.size();
}

int getGPUID(){
return gpuid;
}

void setGPUID(int gid){
gpuid=gid;
}

void setMaxBatch(int size){
	_max_batch=size;
}

int getMaxBatch(){
	return _max_batch;
}

int getNTask(){
return taskRequests.size();
}

void addRequestInfo( request *t){
if (taskRequests.size() >= _max_batch){ 
 printf("number of batch exceeded MAX %d \n", _max_batch);
 exit(1);
}
taskRequests.push_back(t);
batch_num += t->getBatchNum();
if (b_input.size() == 0)
     b_input = t->_input;
else
    b_input.insert(b_input.end(), t->_input.begin(), t->_input.end());
}
void clearBatchInfo(){
	b_input.clear();
	taskRequests.clear();
	batch_num=0;	
}

};

#endif 
