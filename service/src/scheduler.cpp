
#include "scheduler.h"
#include "mon.h"
GlobalScheduler::GlobalScheduler(){
nGPUs=0;
}

GlobalScheduler::~GlobalScheduler(){
endNVML();
}

int GlobalScheduler::setSchedulingMode(string mode){
	if (mode == "no"){
		_mode=NO;
		return 1;
	}
	else if (mode =="greedy"){
		_mode=GREEDY;
		return 1;
	}
	else if (mode=="dedicated"){
		_mode=DEDICATED;
		return 1;
	}
	else if(mode=="proposed"){
		_mode=PROPOSED;
		return 1;
	}
	else if(mode=="cpu_only"){
		_mode=CPU_ONLY;
		return 1;
	}
	else if(mode=="fixed"){
		_mode=FIXED;
		return 1;
	}
	else if(mode=="all_gpu"){
		_mode=ALL_GPU;
		return 1;
	}
	else
		return 0;
		
}

int GlobalScheduler::executeScheduler(const char* req_name, int batchsize){
	switch(_mode){
		case NO:
			return noScheduler();
		case GREEDY:
			return NaiveGreedyScheduler();
		case DEDICATED:
			return dedicatedScheduler(req_name);
		case PROPOSED:
			return proposedScheduler(req_name,batchsize);
		case CPU_ONLY:
			return CPUonlyScheduler();
		case FIXED:
			return fixedScheduler(req_name);
		case ALL_GPU:
			return allGPUScheduler(req_name);
		defaut: // should not happen, conditions are already checked during initialization
			break;
	}
	return 0;
}

/*get adequate gpu based on scheduling*/
/*returns gpu id : 0~npu-1 , or else returns -1 if it needs to wait*/
int GlobalScheduler::proposedScheduler(const char* req_name, int batch_size){
/*to be implemented*/
	/* Greedy + profiled data*/
 	if (gpu_mons.empty()) {
                printf("GlobalScheduler will return 0! Monitor uninitialized!! \n");
                return 0;
        }
#ifdef DEBUG
	printf("Proposed scheduler called\n");
#endif 
	ProcInfo *pinfo = Profile[req_name][batch_size];
        float minUtil=10000.0; //large number 
        int min_index;
        float utilization[4];
	bool needtoWait=true;
        for (int i=0; i<nGPUs; i++)
        {
                getUtilization(*(gpu_mons[i]), utilization);
		//check memory usage
		if (utilization[0] + (pinfo->memUsage)>100)
			continue;
                if (minUtil > utilization[3]+(pinfo->compUtil)){
			needtoWait=false;
                        minUtil=utilization[3]+(pinfo->compUtil);
                        min_index=i;
                }
                //might need to consider memory in the future for now just skip!
        }

	if (needtoWait)
		return -1;
	else
		return min_index;
}

int GlobalScheduler::noScheduler(){
return 0; 
}

int GlobalScheduler::NaiveGreedyScheduler(){
        // iterate through GPUs and find the one that has the most left over computational resources
        if (gpu_mons.empty()) {
                printf("GlobalScheduler will return 0! Monitor uninitialized!! \n");
                return 0;
        }
#ifdef DEBUG
	printf("Naive scheduler called\n");
#endif
        float minUtil=10000.0; //large number 
        int min_index;
        float utilization[4];
        for (int i=0; i<nGPUs; i++)
        {
                getUtilization(*(gpu_mons[i]), utilization);
                if (minUtil > utilization[3]){
                        minUtil=utilization[3];
                        min_index=i;
                }
                //might need to consider memory in the future for now just skip!
        }
        return min_index;


}

int GlobalScheduler::RandomScheduler(){
        srand(time(NULL));
        return rand() % nGPUs ;
}

int GlobalScheduler::CPUonlyScheduler(){
	return nGPUs; // cpu id is identical to the number of GPUs in system
}
void GlobalScheduler::setupMonitor(){
initNVML();
nvmlDevice_t *temp;
for (int i =0; i<nGPUs; i++){
        temp = (nvmlDevice_t*)malloc(sizeof(nvmlDevice_t));
        gpu_mons.push_back(temp);
        initMonitor(i,gpu_mons[i]);
}
}

void GlobalScheduler::setProcInfos(string dir_of_profile_file){

//following file should follow the rules below

//RULES//
//netname,batch size(ascending order), computil,dram usage
//

//Network names should match with those of network names, Ex) imc, pos, ner, face, dig
ifstream infile(dir_of_profile_file);
string line;
string prev_name,cur_name;
string token;
ProcInfo * temp;
prev_name="start!"; //initialized to a name 
while(getline(infile,line)){
        istringstream ss(line);
        int i =0;
        while (getline(ss,token,',')){

                switch(i){
                case 0:
                        cur_name=token;
                        //if new name, make new vector of ProfInfos

                        if (!prev_name.compare(cur_name)){
                        prev_name=token;
                        vector<ProcInfo*> *temp_vector = new vector<ProcInfo*>();
                        Profile[cur_name]= *(temp_vector);
                        }
                        temp=(ProcInfo*)malloc(sizeof(ProcInfo));
                        break;
		case 1:
			//the batchsize itself, Vector<ProfInfo*> will use this as index anyway
			break;
                case 2:
                        temp->compUtil=stof(token,NULL);
			break;
                case 3:
                        temp->memUsage=stof(token,NULL);
			break;
                case 4: // last case 
                        temp->duration=stof(token,NULL);
                        Profile[cur_name].push_back(temp);
			break;
                default:
                        break;
                }
        i++;
        }
}
return;
}

// setups up the maximum batch for each network, for now we are going to use a fixed,static way
void GlobalScheduler::setupMaxBatch(string mb_file){
	ifstream infile(mb_file);
	string line;
	string token;

	while(getline(infile,line)){
		istringstream ss(line);
		int i=0;
		getline(ss,token,',');
		string name = token;		
		getline(ss,token,',');
		string device = token;
		tuple<string, string> temp;
		temp=make_tuple(name,device);
		getline(ss,token,',');
		mb_table[temp]=stoi(token);
		printf("name : %s and device : %s batch size : %d\n", name.c_str(), device.c_str(), mb_table[temp]);
	}


}

void GlobalScheduler::setNGPUs(int nGPU){
nGPUs=nGPU;
}

void GlobalScheduler::printProfileInfo(){
int nName = Profile.size(); // get total number of Names profile
printf("total %d networks profiled ! \n ", nName);

for (map<string , vector<ProcInfo*>>::iterator  mit = Profile.begin();  mit != Profile.end(); mit++){
	int batch_size = 0;
	for (vector<ProcInfo*>:: iterator vit = mit->second.begin(); vit != mit->second.end(); vit++){
		printf("%s , batch: %d,  compUtil: %f, memUsage: %f, duration: %f \n", mit->first.c_str(), batch_size,(*vit)->compUtil, (*vit)->memUsage,(*vit)->duration);
		batch_size++;
		}
	}
	printf("end of profiled infos ! \n");
}


int GlobalScheduler::dedicatedScheduler(const char* req_name){
	map<string, int>::iterator it;
	string str_name(req_name);
	it = device_table.find(str_name);
	if( it == device_table.end()){ //setup dedicated scheduler if not found
		device_table[str_name]=(device_table.size())%nGPUs;
		printf("assigning %s to gpu %d\n",str_name.c_str(),device_table[str_name]);
		
	}
	it = device_table.find(str_name);
	return it->second;
}

int GlobalScheduler::fixedScheduler(const char* req_name){
	map<string, int>::iterator it;
	string str_name(req_name);
	it = device_table.find(str_name);
	if( it == device_table.end()){ //setup device table if not found
		/*below are fixed tables*/
		if (!(str_name.compare("pos") )) device_table[str_name]=nGPUs; // set to CPU
		else if (!(str_name.compare("face"))) device_table[str_name]=0;
		else if (!(str_name.compare("imc"))) device_table[str_name]=1;
		else if (!(str_name.compare("dig"))) device_table[str_name]=2;
		else if (!(str_name.compare("chk"))) device_table[str_name]=nGPUs;
		else if (!(str_name.compare("ner"))) device_table[str_name]=nGPUs;


printf("assigning %s to gpu %d\n",str_name.c_str(),device_table[str_name]);
		
	}
	it = device_table.find(str_name);
	return it->second;
}


int GlobalScheduler::getMaxBatch(string name , string type){
	// the following should not fail if setup was called correctly
	map<tuple<string, string>, int>::iterator it;
	tuple<string, string> query = make_tuple(name, type);
	it=mb_table.find(query);
	return it->second;
}

int GlobalScheduler::allGPUScheduler(const char* req_name){
	map<string, int>::iterator it;
	string str_name(req_name);
	it = device_table.find(str_name);
	if( it == device_table.end()){ //setup device table if not found
		/*below are fixed tables*/
		if (!(str_name.compare("pos") )) device_table[str_name]=2; // set to CPU
		else if (!(str_name.compare("face"))) device_table[str_name]=0;
		else if (!(str_name.compare("imc"))) device_table[str_name]=1;
		else if (!(str_name.compare("dig"))) device_table[str_name]=2;
		else if (!(str_name.compare("chk"))) device_table[str_name]=3;
		else if (!(str_name.compare("ner"))) device_table[str_name]=3;


printf("assigning %s to gpu %d\n",str_name.c_str(),device_table[str_name]);
		
	}
	it = device_table.find(str_name);
	return it->second;
}

