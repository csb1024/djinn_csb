/*
 *  Copyright (c) 2015, University of Michigan.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <map>
#include <glog/logging.h>
#include <time.h>
#include <nvml.h>
#include <queue>
#include <vector>


#include "boost/program_options.hpp"
#include "socket.h"
#include "scheduler.h"
#include "thread.h"
#include "tonic.h"
//#include "ts_queue.h"
#include "ts_list.h"
#include "request.h"
#include "batched_request.h"
#include "mon.h"


using namespace std;
namespace po = boost::program_options;

GlobalScheduler gsc;
bool debug;
bool gpu;
unsigned int task_id;
FILE* log_file;
int CPUID;
exec_mode gexec_mode;
exec_mode cexec_mode;

//file + dir that containts required files
string common;
string weight_dir;
string net_list;


//used for simple scheduler
int gpu_turn;



static uint64_t getCurNs() {
   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   uint64_t t = ts.tv_sec*1000*1000*1000 + ts.tv_nsec;
   return t;
}



string uint64_to_string( uint64_t value ) {
    ostringstream os;
    os << value;
    return os.str();
}

po::variables_map parse_opts(int ac, char** av) {
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "Produce help message")(
      "common,c", po::value<string>()->default_value("../common/"),
      "Directory with configs and weights")(
      "portno,p", po::value<int>()->default_value(8080),
      "Port to open DjiNN on")
      ("nets,n", po::value<string>()->default_value("nets.txt"),
       "File with list of network configs (.prototxt/line)")
("weights,w", po::value<string>()->default_value("weights/"),"Directory containing weights (in common)")

          ("gpu,g", po::value<bool>()->default_value(false), "Use GPU?")
("debug,v", po::value<bool>()->default_value(false),"Turn on all debug")
("threadcnt,t",po::value<int>()->default_value(-1),"Number of threads to spawn before exiting the server. (-1 loop forever)")
    ("ngpu,ng", po::value<int>()->default_value(4),"number of gpus in system to run service")
    ("scheduler,s",po::value<string>()->default_value("no"),"scheduling mode : no, greedy, dedicated, proposed")
    ("profile,prof",po::value<string>()->default_value("profile.csv"),"the file which stores profile value of each network" )
("cpu_exec,cexec",po::value<string>()->default_value("PEB"),"the execution mode of CPU : CON,B2B,PEB,EB" )
("gpu_exec,gexec",po::value<string>()->default_value("PEB"),"the execution mode of GPU : CON,B2B,PEB,EB" )
;

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << "\n";
    exit(1);
  }
  return vm;
}




int main(int argc, char* argv[]) {
  // Main thread for the server
  // Spawn a new thread for each request
  uint64_t start, end;
  po::variables_map vm = parse_opts(argc, argv);

  debug = vm["debug"].as<bool>();
  gpu = vm["gpu"].as<bool>();

  // will need better way, for now lets just hard code    
  int numofGPU=vm["ngpu"].as<int>();

  // set the CPU ID to be number of GPUs
  CPUID=numofGPU;
  
  


  // global variable needed to load all models to gpus at init
  common = vm["common"].as<string>();
  net_list = vm["nets"].as<string>();
  weight_dir = vm["weights"].as<string>();

 //execution mode for CPU & GPU server 
  // setup exec_mode table
  map<string, exec_mode> exec_table;
  exec_table["B2B"]=B2B;
  exec_table["CON"]=CON;
  exec_table["EB"]=EB;
  exec_table["PEB"]=PEB;

  cexec_mode = exec_table[vm["cpu_exec"].as<string>()];
  gexec_mode = exec_table[vm["gpu_exec"].as<string>()];

 
   // ifstream file(nets.c_str());
//  start = getCurNs(); 
  pthread_t tid[numofGPU+1];
  if (vm["gpu"].as<bool>()){
  //     Caffe::set_mode(Caffe::GPU);
       for (int i =0; i<numofGPU; i++)
	{
	    tid[i]=server_thread_init(i);
	// important that we sleep for synch reasons
	    sleep(3);
	}
	// init scheduler
        string mode=vm["scheduler"].as<string>();
        if (!gsc.setSchedulingMode(mode)){
		printf("exiting server due to undefined scheduling mode\n");
		exit(1);
	}

	gsc.setNGPUs(numofGPU);
	gsc.setupMonitor();
	string profile_dir= vm["profile"].as<string>();
	gsc.setProcInfos(profile_dir);
	gsc.setupMaxBatch(string("SLA.txt")); // for now lets just hard code the file name 
  }
//    Caffe::set_mode(Caffe::CPU);
    //init CPU Server
    tid[numofGPU]=server_thread_init(CPUID); //init single server for CPU
  Caffe::set_phase(Caffe::TEST);
  
//initiate threads and scheduler/monitor
//  batching_thread_init(); // start batching thread
 clear_thread_init();
 // batch_table_thread_init(); // maintian tasks to batch in a table MUST BE CALLED AFTER server_thread_init
  log_file=fopen("log.txt","w");
  fprintf(log_file,"task,taskID,REQ_TO_RECVQ,RECVQ_TO_BATCHQ,EXEC,PREP_EXEC,BQ_TO_EXEC\n");
  fflush(log_file);

 // how many threads to spawn before exiting
  // -1 to stay indefinitely open
   int thread_cnt = 0;

  int total_thread_cnt = vm["threadcnt"].as<int>();
  int socketfd = SERVER_init(vm["portno"].as<int>());

  // Listen on socket
  listen(socketfd, 10);

    cout << "Server is listening for requests on " << vm["portno"].as<int>()<<endl;


   // Main Loop
 
  while (1) {
 
    pthread_t new_thread_id;
      int client_sock = accept(socketfd, (sockaddr*)0, (unsigned int*)0);
#ifdef DEBUG
      cout << "accepting new socket , "<<client_sock << endl; 
#endif 
  
    if (client_sock == -1) {
      LOG(ERROR) << "Failed to accept.\n";
      continue;
    }    
    new_thread_id = request_thread_init(client_sock);
     

    ++thread_cnt;
    if (thread_cnt == total_thread_cnt) {
      if (pthread_join(new_thread_id, NULL) != 0) {
        LOG(FATAL) << "Failed to join.\n";
      }
      break;
    }
  }

  //endNVML();
  return 0;
}
