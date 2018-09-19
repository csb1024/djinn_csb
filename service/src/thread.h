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
#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <mutex>

#include "caffe/caffe.hpp"
#include "socket.h"
#include "tonic.h"
#include "request.h"
#include "batched_request.h"

using caffe::Blob;
using caffe::Caffe;
using caffe::Net;
using caffe::Layer;
using caffe::shared_ptr;
using caffe::Timer;
using caffe::vector;



using namespace std;

struct arg_struct {
  int datalen;
  int gpu_id;
  char  *req_name;
  float *in_data;
  
};


/*
1. B2B = back to back 
2. CON = execute all reqeusts concurrently 
3. EB = eager batching, execute batch as soon as previous batch is done
4. PEB = pure eager batching, no limit on maximum batch size
*/

enum exec_mode {B2B, CON, EB, PEB}; // used for deciding whether or not to execute a batched request, 

pthread_t request_thread_init(int sock);
//pthered_t request_med_thread_init(int sock, int gpu_id);
void* request_handler(void* sock);

pthread_t execution_thread_init(batched_request *req);
void* execution_handler(void *args);

pthread_t batching_thread_init();
void* batch_handler(void *args);

pthread_t server_thread_init(int gpu_id);
void* server_init(void* gpu_id);

pthread_t clear_thread_init();
void *clear_handler();

pthread_t batch_table_thread_init();
void *btable_handler();


#endif // #define _THREAD_H_
