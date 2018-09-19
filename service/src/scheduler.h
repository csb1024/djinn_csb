#ifndef SCHED_H
#define SCHED_H


#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <nvml.h>
#include <map>

using namespace std;

typedef struct {
float compUtil;
float memUsage;
float duration; 
} ProcInfo;


enum scheduler {NO, GREEDY, DEDICATED, PROPOSED, CPU_ONLY, FIXED, ALL_GPU};
class GlobalScheduler
{

public: 
GlobalScheduler();
~GlobalScheduler();


/*schedulers
 *all of the following returns int as gpu id
 */

int executeScheduler(const char *req_name, int batchsize);
int proposedScheduler(const char *req_name, int batch_size);
int NaiveGreedyScheduler(); // returns id based on greedy scheduling
int RandomScheduler(); // returns random number based on the number of GPUS
int noScheduler(); //returns 0
int dedicatedScheduler(const char *req_name);
int fixedScheduler(const char *req_name);
int allGPUScheduler(const char *req_name);
int CPUonlyScheduler();



/*methods*/
void setProcInfos(string dir_of_profile_file); // sets up profiling info by given directory
void setNGPUs(int nGPU);
void setupMonitor();
int setSchedulingMode(string mode);
void setupMaxBatch(string mb_file);
void printProfileInfo(); // for debuggung
int getMaxBatch(string name, string device); // get max number of batch, given request name and device type
private: 
scheduler _mode;
map<string, vector<ProcInfo*>> Profile; // indexed first name and then batchsize
int nGPUs;
vector<nvmlDevice_t *> gpu_mons;
map<string,int> device_table; //table used for dedicated scheduler
map<tuple<string, string>, int> mb_table;//max batch table 

};

#endif 
