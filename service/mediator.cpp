/*
the mediator, contains routing,scheduling and sending control messeages to servers that control  GPU
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
#include<vector>

#include "boost/program_options.hpp"
#include "socket.h"
#include "thread.h"
#include "tonic.h"

/*returns port number, returns 0 if something is wrong*/
int retServerPort(int socknum, int *gpu_map, int nGPU, int util[][2] ){ // accepts connection from client and returns port number based on scheduling result


if(gpu_map == NULL || nGPU == 0){
printf("please check mapping array and number of GPUs passed!\n");
return 0;
}

if (util == NULL){
printf("empty utilization array!\n");
return 0;
}


/*for local debugging purposes */
int local_debug = 1;

 char req_name[MAX_REQ_SIZE];  
 SOCKET_receive(socknum, (char*)&req_name, MAX_REQ_SIZE, local_debug);
if (local_debug)
 printf("received %s from client \n ", req_name);

/*need to find network name among networks stored priorly*/
/*for now lets just skip*/
/*map<string, Net<float>*>::iterator it = nets.find(req_name);
if (it == nets.end())
	LOG(ERROR) << "Task " << req_name << " not found.";
else
	LOG(INFO) << "Task " << req_name << " forward pass.";*/


// recieve batch size 
int batch_size = SOCKET_rxsize(socknum);
if (local_debug)
 printf("batch size : %d \n",batch_size);


/*need to implement scheduler right here*/
/*for now lets just return the last GPU among */
int server = gpu_map[nGPU-1];
SOCKET_txsize(socknum,server);

// clost connection to client
close(socknum);
return 1;
}

int main(int argc, char* argv[])
{

  int gpu_id = 0;
  int gpu_to_port[4];
  int task_id = 0;
  int server_port_num;
  // will need better way, for now lets just hard code 
  int numofGPU=4;
  gpu_to_port[0]=8081;
  gpu_to_port[1]=8082;
  gpu_to_port[2]=8083;
  gpu_to_port[3]=8084;
 
//utilization related info
  int gpuUtil[4][2]={0,};
  gpuUtil[0][0]=1;
  gpuUtil[1][0]=2;
  gpuUtil[2][0]=3;
  gpuUtil[3][0]=4;

  po::variables_map vm = parse_opts(argc, argv);

  int total_thread_cnt = vm["threadcnt"].as<int>();
  int socketfd = SERVER_init(vm["portno"].as<int>());

  // Listen on socket
  listen(socketfd, 10);
  LOG(INFO) << "Server is listening for requests on " << vm["portno"].as<int>();

  // Main Loop
  int thread_cnt = 0;
  while (1) {
    pthread_t new_thread_id;
    int client_sock = accept(socketfd, (sockaddr*)0, (unsigned int*)0);

    if (client_sock == -1) {
      LOG(ERROR) << "Failed to accept.\n";
      continue;
    } 


// gather utilization info of GPU
     /*needs to be done in future*/
   
    if(!sendServerPort(client_sock, gpu_to_port, numofGPU, gpuUtil))
	LOG(ERROR) << "Failed to send server port\n"; 

     
    ++thread_cnt;
    if (thread_cnt == total_thread_cnt) {
      if (pthread_join(new_thread_id, NULL) != 0) {
        LOG(FATAL) << "Failed to join.\n";
      }
      break;
    }
  }
  return 0;


}



