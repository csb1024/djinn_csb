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
#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <sys/time.h>
#include <chrono>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "opencv2/opencv.hpp"
#include "boost/program_options.hpp"
#include "caffe/caffe.hpp"
#include "align.h"
#include "socket.h"
#include "tonic.h"
#include "client_model.h"
#ifdef MPIVER
#include "mpi.h"
#endif

using namespace std;
using namespace cv;

namespace po = boost::program_options;


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
      "common,c", po::value<string>()->default_value("../../common/"),
      "Directory with configs and weights")(
      "task,t", po::value<string>()->default_value("imc"),
      "Image task: imc (ImageNet), face (DeepFace), dig (LeNet)")(
      "network,n", po::value<string>()->default_value("imc.prototxt"),
      "Network config file (.prototxt)")(
      "weights,w", po::value<string>()->default_value("imc.caffemodel"),
      "Pretrained weights (.caffemodel)")(
      "input,i", po::value<string>()->default_value("imc-list.txt"),
      "List of input images (1 jpg/line)")

      ("djinn,d", po::value<bool>()->default_value(false),"Use DjiNN service?")
      ("hostname,o",po::value<string>()->default_value("localhost"), 
      "Server IP addr")
      ("portno,p", po::value<int>()->default_value(8080), "Server port")
      ("batch,b", po::value<int>()->default_value(3),"size of batch to send")
      // facial recognition flags
      ("align,l", po::value<bool>()->default_value(true),
       "(face) align images before inference?")(
          "haar,a", po::value<string>()->default_value("data/haar.xml"),
          "(face) Haar Cascade model")
      ("flandmark,f",po::value<string>()->default_value("data/flandmark.dat"),
          "(face) Flandmarks trained data")
      ("gpu,g", po::value<bool>()->default_value(false), "Use GPU?")
      ("debug,v", po::value<bool>()->default_value(false),"Turn on all debug")
      ("requests,r",po::value<int>()->default_value(1),"how many requests are going to be issued to the server" )
      ("mean,m,",po::value<double>()->default_value(0.3),"how long is the average time between each request(in seconds)");


  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << "\n";
    exit(1);
  }
  return vm;
}

int main(int argc, char** argv) {
//  google::InitGoogleLogging(argv[0]);
 // google::minloglevel(2);
  po::variables_map vm = parse_opts(argc, argv);
  bool debug = vm["debug"].as<bool>();
  uint64_t times[6]={0,};
//  uint64_t end_barrier_time = 0;

  int pid = getpid();
  TonicSuiteApp app;
  int rank, nprocs;
  //default number of processes =1
  nprocs=1;
  double mean = vm["mean"].as<double>();

#ifdef MPIVER
 
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs); 
  MPI_Comm_rank(MPI_COMM_WORLD,&rank); 
#endif
  client_model *rand_model = new client_model(nprocs, 1/mean);

  app.task = vm["task"].as<string>();
  app.network =
      vm["common"].as<string>() + "configs/" + vm["network"].as<string>();
  app.weights =
      vm["common"].as<string>() + "weights/" + vm["weights"].as<string>();
  app.input = vm["input"].as<string>();
  int batch_size = vm["batch"].as<int>();
  int nrequests = vm["requests"].as<int>();
  // DjiNN service or local?
  app.djinn = vm["djinn"].as<bool>();
  app.gpu = vm["gpu"].as<bool>();


  times[0] = getCurNs();
  if (app.djinn) {
      app.hostname = vm["hostname"].as<string>();
    app.portno = vm["portno"].as<int>();
    app.socketfd = CLIENT_init((char*)app.hostname.c_str(), app.portno, debug);
    if (app.socketfd < 0) exit(0);
//    LOG(INFO) << "PRE Time(ns): " << uint64_to_string(end-start) << endl;
  } else {
    app.net = new Net<float>(app.network);
    app.net->CopyTrainedLayersFrom(app.weights);
    Caffe::set_phase(Caffe::TEST);
      if (app.gpu)
      Caffe::set_mode(Caffe::GPU);
    else
      Caffe::set_mode(Caffe::CPU);

  // LOG(INFO) << "PRE Time(ns): " << uint64_to_string(end-start) << endl;
  }
  times[1]=getCurNs();
#ifdef MPIVER
  MPI_Barrier(MPI_COMM_WORLD);
#endif
   times[2] = getCurNs();
 strcpy(app.pl.req_name, app.task.c_str());
vector<pair<string, Mat> > imgs;
  std::ifstream file(app.input.c_str());
  std::string img_file;
  app.pl.num = 0;


  app.pl.size = 0;
  // hardcoded for AlexNet
  if (app.task == "imc") app.pl.size = 3 * 227 * 227;
  // hardcoded for DeepFace
  else if (app.task == "face")
    app.pl.size = 3 * 152 * 152;
  // hardcoded for Mnist
  else if (app.task == "dig")
    app.pl.size = 1 * 28 * 28;
  else
    LOG(FATAL) << "Unrecognized task.\n";


for (int i =0; i<batch_size;i++)
  {
    if ( !getline(file, img_file)) break;
    LOG(INFO) << "Reading " << img_file;
    Mat img;
    if (app.task == "dig")
      img = imread(img_file, CV_LOAD_IMAGE_GRAYSCALE);
    else
      img = imread(img_file);

    if (img.channels() * img.rows * img.cols != app.pl.size)
      LOG(ERROR) << "Skipping " << img_file
                 << ", resize to correct dimensions.\n";
    else {
      imgs.push_back(make_pair(img_file, img));
      ++app.pl.num;
    }
  }
  if (app.pl.num < 1) LOG(FATAL) << "No images read!";
  vector<pair<string, Mat> >::iterator it;
  // align facial recognition image
  if (app.task == "face" && vm["align"].as<bool>()) {
    for (it = imgs.begin(); it != imgs.end(); ++it) {
      LOG(INFO) << "Aligning: " << it->first << endl;
      preprocess(it->second, vm["flandmark"].as<string>(),
                 vm["haar"].as<string>());
      // comment in save + view aligned image
      // imwrite(it->first+"_a", it->second);
    }
  }
  // prepare data into array
  app.pl.data = (float*)malloc(app.pl.num * app.pl.size * sizeof(float));
  float* preds = (float*)malloc(app.pl.num * sizeof(float));

  int img_count = 0;
  for (it = imgs.begin(); it != imgs.end(); ++it) {
    int pix_count = 0;
    for (int c = 0; c < it->second.channels(); ++c) {
      for (int i = 0; i < it->second.rows; ++i) {
        for (int j = 0; j < it->second.cols; ++j) {
          Vec3b pix = it->second.at<Vec3b>(i, j);
          float* p = (float*)(app.pl.data);
          p[img_count * app.pl.size + pix_count] = pix[c];
          ++pix_count;
        }
      }
    }
    ++img_count;
  }
  times[3]= getCurNs(); //data preparation  




if(app.djinn) {
	SOCKET_send(app.socketfd, (char*)&app.pl.req_name, MAX_REQ_SIZE, debug);
        SOCKET_txsize(app.socketfd, batch_size);
        SOCKET_txsize(app.socketfd, app.pl.num * app.pl.size);
}

  //  while (getline(file, img_file)) 
  
for(int i=0; i<nrequests;i++){

 // usleep(30000*rank);
     double rand_interval;

    // rand_interval=rand_model->getRandNumber();
    rand_interval  = rand_model->randomExponentialInterval(mean);

     printf("DEBUG: random interval : %lf \n",rand_interval);
     sleep(rand_interval);
//  SOCKET_send(app.socketfd, (char*)&app.pl.req_name, MAX_REQ_SIZE, debug);
    // send batch size 
   // app.portno=SOCKET_rxsize(app.socketfd);

 // printf("recieved port %d from mediator \n",app.portno);
  // read in images
  // cmt: using map, cant use duplicate names for images
  // change to other structure (vector) if you want to send the same exact
  // filename multiple times
  if (app.djinn) {
    // send len
      // send image(s)
    SOCKET_send(app.socketfd, (char*)app.pl.data,
                app.pl.num * app.pl.size * sizeof(float), debug);

    times[4]=getCurNs(); //SEND Data
  // LOG(INFO) << "data needs to be recieved!! (can you see me??)" << endl;
    
 /*  SOCKET_receive(app.socketfd, (char*)preds, app.pl.num * sizeof(float), debug);
  for (it = imgs.begin(); it != imgs.end(); it++) {
    LOG(INFO) << "Image: " << it->first
              << " class: " << preds[distance(imgs.begin(), it)] << endl;
    ;
  }*/

    times[5]=getCurNs(); //SERVER EXEC
       //auto end = chrono::steady_clock::now();
  //   LOINFO) << "COM Time(ns): " << uint64_to_string(end-start) << endl;
  } else {
    float loss;
    reshape(app.net, app.pl.num * app.pl.size);
    vector<Blob<float>*> in_blobs = app.net->input_blobs();
    in_blobs[0]->set_cpu_data((float*)app.pl.data);
    times[4] = getCurNs();
    vector<Blob<float>*> out_blobs = app.net->ForwardPrefilled(&loss);
    times[5]=getCurNs(); //EXEC 
    memcpy(preds, out_blobs[0]->cpu_data(), app.pl.num * sizeof(float));
    //LOG(INFO)<<"COM Time(ns): " << uint64_to_string(end-start) <<endl;
  }
}
if (app.djinn) SOCKET_close(app.socketfd, debug);
  free(app.pl.data);
  free(preds);
if (!app.djinn) free(app.net);
  

#ifdef MPIVER
    LOG(ERROR) << "RANK: " << rank << " "<<pid<<endl;
#endif
    for( int i=0; i<6; i++){
    switch(i){
    case 0:
	LOG(ERROR) << "START Time(ns): " << uint64_to_string(times[i]) << " " <<pid<<endl;
	break;
    case 1:
	LOG(ERROR) << "END PRE Time(ns): " << uint64_to_string(times[i]) << " " <<pid<<endl;
	break;
    case 2:
	LOG(ERROR) << "END BARRIER Time(ns): " << uint64_to_string(times[i]) << " "<<pid<<endl;
	break;

    case 3:
	LOG(ERROR) << "END DATA Time(ns): " << uint64_to_string(times[i]) << " "<<pid<<endl;
	break;

    case 4: 
	LOG(ERROR) << "END SEND Time(ns): " << uint64_to_string(times[i]) << " " <<pid<<endl;
	break;
    case 5: 
	LOG(ERROR) << "END COM Time(ns): " << uint64_to_string(times[i]) << " " <<pid<<endl;
	break;

    default: 
       printf("You are not supposed to see this!! \n");
	
        }
   }

 #ifdef MPIVER
  MPI_Finalize();
#endif

  return 0;
}
