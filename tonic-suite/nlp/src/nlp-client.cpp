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
#include <glog/logging.h>
#include <chrono>
#include <time.h>

#include "boost/program_options.hpp"

#include "SENNA_utils.h"
#include "SENNA_Hash.h"
#include "SENNA_Tokenizer.h"
#include "SENNA_POS.h"
#include "SENNA_CHK.h"
#include "SENNA_NER.h"

#include "socket.h"
#include "tonic.h"
#include "client_model.h"

/* fgets max sizes */
#define MAX_TARGET_VB_SIZE 256

using namespace std;
namespace po = boost::program_options;

bool debug;


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

po::variables_map parse_opts(int ac, char **av) {
  // Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()("help,h", "Produce help message")(
	"common,c", po::value<string>()->default_value("../../common/"),
	"Directory with configs and weights")(
	"task,t", po::value<string>()->default_value("pos"),
	"Image task: pos (Part of speech tagging), chk (Chunking), ner (Name "
	"Entity Recognition)")("network,n",
			     po::value<string>()->default_value("pos.prototxt"),
			     "Network config file (.prototxt)")(
	"weights,w", po::value<string>()->default_value("pos.caffemodel"),
	"Pretrained weights (.caffemodel)")(
	"input,i", po::value<string>()->default_value("input/small-input.txt"),
	"File with text to analyze (.txt)")

	("djinn,d", po::value<bool>()->default_value(false),
	"Use DjiNN service?")("hostname,o",
			     po::value<string>()->default_value("localhost"),
			     "Server IP addr")
	("portno,p", po::value<int>()->default_value(8080), "Server port")
	("batch,b",po::value<int>()->default_value(3),"batch size")


	("gpu,g", po::value<bool>()->default_value(false), "Use GPU?")(
	      "debug,v", po::value<bool>()->default_value(false),
	      "Turn on all debug")
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

int main(int argc, char *argv[]) {
  // google::InitGoogleLogging(argv[0]);


  po::variables_map vm = parse_opts(argc, argv);

  /* SENNA Inits */
  /* options */
  char *opt_path = NULL;
  int opt_usrtokens = 0;
  int batch_size = vm["batch"].as<int>();
  int nrequests = vm["requests"].as<int>();
  /* the real thing */
  char target_vb[MAX_TARGET_VB_SIZE];
  int *chk_labels = NULL;
  int *pt0_labels = NULL;
  int *pos_labels = NULL;
  int *ner_labels = NULL;

  /* timing related variables*/
  uint64_t start;
  uint64_t end;
  uint64_t times[10] = {0,};

  /* inputs */
  SENNA_Hash *word_hash = SENNA_Hash_new(opt_path, "hash/words.lst");
  SENNA_Hash *caps_hash = SENNA_Hash_new(opt_path, "hash/caps.lst");
  SENNA_Hash *suff_hash = SENNA_Hash_new(opt_path, "hash/suffix.lst");
  SENNA_Hash *gazt_hash = SENNA_Hash_new(opt_path, "hash/gazetteer.lst");

  SENNA_Hash *gazl_hash = SENNA_Hash_new_with_admissible_keys(
      opt_path, "hash/ner.loc.lst", "data/ner.loc.dat");
  SENNA_Hash *gazm_hash = SENNA_Hash_new_with_admissible_keys(
      opt_path, "hash/ner.msc.lst", "data/ner.msc.dat");
  SENNA_Hash *gazo_hash = SENNA_Hash_new_with_admissible_keys(
      opt_path, "hash/ner.org.lst", "data/ner.org.dat");
  SENNA_Hash *gazp_hash = SENNA_Hash_new_with_admissible_keys(
      opt_path, "hash/ner.per.lst", "data/ner.per.dat");

  /* labels */
  SENNA_Hash *pos_hash = SENNA_Hash_new(opt_path, "hash/pos.lst");
  SENNA_Hash *chk_hash = SENNA_Hash_new(opt_path, "hash/chk.lst");
  SENNA_Hash *ner_hash = SENNA_Hash_new(opt_path, "hash/ner.lst");

  // weights not used
  SENNA_POS *pos = SENNA_POS_new(opt_path, "data/pos.dat");
  SENNA_CHK *chk = SENNA_CHK_new(opt_path, "data/chk.dat");
  SENNA_NER *ner = SENNA_NER_new(opt_path, "data/ner.dat");

  /* tokenizer */
  SENNA_Tokenizer *tokenizer =
      SENNA_Tokenizer_new(word_hash, caps_hash, suff_hash, gazt_hash, gazl_hash,
                          gazm_hash, gazo_hash, gazp_hash, opt_usrtokens);

  /* Tonic Suite inits */
  TonicSuiteApp app;
  debug = vm["debug"].as<bool>();
  app.task = vm["task"].as<string>();
  app.network =
      vm["common"].as<string>() + "configs/" + vm["network"].as<string>();
  app.weights =
      vm["common"].as<string>() + "weights/" + vm["weights"].as<string>();
  app.input = vm["input"].as<string>();

  // DjiNN service or local?
  app.djinn = vm["djinn"].as<bool>();
  app.gpu = vm["gpu"].as<bool>();

  // Init client model
    //default number of processes =1
  int nprocs=1;
  double mean = vm["mean"].as<double>();
  client_model *rand_model = new client_model(nprocs, 1/mean);




  if (app.djinn) {
   start = getCurNs();
    app.hostname = vm["hostname"].as<string>();
    app.portno = vm["portno"].as<int>();
    app.socketfd = CLIENT_init(app.hostname.c_str(), app.portno, debug);
    if (app.socketfd < 0) exit(0);
  end = getCurNs();
  } else {
   start=getCurNs();
    app.net = new Net<float>(app.network);
    app.net->CopyTrainedLayersFrom(app.weights);
   end=getCurNs();
    if (app.gpu)
      Caffe::set_mode(Caffe::GPU);
    else
      Caffe::set_mode(Caffe::CPU);
  }
times[0]=end-start;
start=getCurNs();
  strcpy(app.pl.req_name, app.task.c_str());
  if (app.task == "pos")
    app.pl.size = pos->window_size *
                  (pos->ll_word_size + pos->ll_caps_size + pos->ll_suff_size);
  else if (app.task == "chk") {
    app.pl.size = chk->window_size *
                  (chk->ll_word_size + chk->ll_caps_size + chk->ll_posl_size);
  } else if (app.task == "ner") {
    int input_size = ner->ll_word_size + ner->ll_caps_size + ner->ll_gazl_size +
                     ner->ll_gazm_size + ner->ll_gazo_size + ner->ll_gazp_size;
    app.pl.size = ner->window_size * input_size;
  }

  // read input file
  ifstream file(app.input.c_str());
  string str;
  string text;
  for (int i =0; i<batch_size;i++){ 
  if (getline(file, str)) text += str;
  else break;
  
  }

  // tokenize
  SENNA_Tokens *tokens = SENNA_Tokenizer_tokenize(tokenizer, text.c_str());
  app.pl.num = tokens->n;

  if (app.pl.num == 0) LOG(FATAL) << app.input << " empty or no tokens found.";
  end=getCurNs();
  times[1]=end-start;
  if (app.task == "pos") {

//  auto start = chrono::steady_clock::now();
    start = getCurNs();

    if (app.djinn) {
       // send app
      
      SOCKET_send(app.socketfd, (char *)&app.pl.req_name, MAX_REQ_SIZE, debug);
	//send batch size(# of tokens)
      SOCKET_txsize(app.socketfd, app.pl.num );

      // send len
      SOCKET_txsize(app.socketfd, app.pl.num * app.pl.size);
    } else
      reshape(app.net, app.pl.num * app.pl.size);
   end = getCurNs();
   times[2]=end-start;
   start =getCurNs();
for(int i=0; i<nrequests;i++){
     double rand_interval;
     rand_interval  = rand_model->randomExponentialInterval(mean);
    printf("DEBUG: random interval : %lf \n",rand_interval);
    sleep(rand_interval);

    pos_labels = SENNA_POS_forward(pos, tokens->word_idx, tokens->caps_idx,
                                   tokens->suff_idx, app);
}
  //  auto end = chrono::steady_clock::now();
    end = getCurNs();
//    LOG(INFO) << "Time(ns): " << uint64_to_string(end-start) << endl;
    times[3]=end-start; 
  } else if (app.task == "chk") {
    // chk needs internal pos
   start = getCurNs();
    TonicSuiteApp pos_app = app;
    pos_app.task = "pos";
    pos_app.network = vm["common"].as<string>() + "configs/" + "pos.prototxt";
    pos_app.weights = vm["common"].as<string>() + "weights/" + "pos.caffemodel";

    if (!pos_app.djinn) {
      pos_app.net = new Net<float>(pos_app.network);
      pos_app.net->CopyTrainedLayersFrom(pos_app.weights);
    }
    strcpy(pos_app.pl.req_name, pos_app.task.c_str());
    pos_app.pl.size =
        pos->window_size *
        (pos->ll_word_size + pos->ll_caps_size + pos->ll_suff_size);

    // send pos app
    if (pos_app.djinn) {
      pos_app.socketfd =
          CLIENT_init(pos_app.hostname.c_str(), pos_app.portno, debug);
      SOCKET_send(pos_app.socketfd, (char *)&pos_app.pl.req_name, MAX_REQ_SIZE,
                  debug);
      //send batch size(token size)
      SOCKET_txsize(pos_app.socketfd,pos_app.pl.num);
      // send len
      SOCKET_txsize(pos_app.socketfd, pos_app.pl.num * pos_app.pl.size);
    } else
      reshape(pos_app.net, pos_app.pl.num * pos_app.pl.size);
    pos_labels = SENNA_POS_forward(pos, tokens->word_idx, tokens->caps_idx,
                                   tokens->suff_idx, pos_app);



    SOCKET_close(pos_app.socketfd, debug);
  end = getCurNs();
   times[0] += (end-start);//add to preparing time
    // chk foward pass
 //   auto start = chrono::steady_clock::now();
    start = getCurNs();

    if (app.djinn) {
      SOCKET_send(app.socketfd, (char *)&app.pl.req_name, MAX_REQ_SIZE, debug);
      // send batch size
      SOCKET_txsize(app.socketfd, app.pl.num);
      // send len
      SOCKET_txsize(app.socketfd, app.pl.num * app.pl.size);
    } else {
      free(pos_app.net);
      reshape(app.net, app.pl.num * app.pl.size);
    }
  end = getCurNs();
  times[2]=end-start; 
  start = getCurNs();
for(int i=0; i<nrequests;i++){
     double rand_interval;
     rand_interval  = rand_model->randomExponentialInterval(mean);
    printf("DEBUG: random interval : %lf \n",rand_interval);
    sleep(rand_interval);


    chk_labels = SENNA_CHK_forward(chk, tokens->word_idx, tokens->caps_idx,
                                   pos_labels, app);
}
 //   auto end = chrono::steady_clock::now();
    end = getCurNs();
  times[3] = end -start; 
  //  LOG(INFO) << "Time(ns): " << uint64_to_string(end-start)<< endl;

  } else if (app.task == "ner") {
   // auto start = chrono::steady_clock::now();
    start = getCurNs();
    if (app.djinn) {

      // send app
      SOCKET_send(app.socketfd, (char *)&app.pl.req_name, MAX_REQ_SIZE, debug);
      // send batch size
      SOCKET_txsize(app.socketfd,app.pl.num);
      // send len
      SOCKET_txsize(app.socketfd, app.pl.num * app.pl.size);
    } else
      reshape(app.net, app.pl.num * app.pl.size);
   end=getCurNs();
   times[2]=end-start;
  start=getCurNs();
for(int i=0; i<nrequests;i++){
     double rand_interval;
     rand_interval  = rand_model->randomExponentialInterval(mean);
    printf("DEBUG: random interval : %lf \n",rand_interval);
    sleep(rand_interval);

    ner_labels = SENNA_NER_forward(ner, tokens->word_idx, tokens->caps_idx,
                                   tokens->gazl_idx, tokens->gazm_idx,
                                   tokens->gazo_idx, tokens->gazp_idx, app);
}
//  auto end = chrono::steady_clock::now();
   end = getCurNs();
 times[3]=end-start;
//  LOG(INFO) << "Time(ns): " << uint64_to_string(end-start)<< endl;
  }

  for (int i = 0; i < tokens->n; i++) {
    printf("%15s", tokens->words[i]);
    if (app.task == "pos")
      printf("\t%10s", SENNA_Hash_key(pos_hash, pos_labels[i]));
    else if (app.task == "chk")
      printf("\t%10s", SENNA_Hash_key(chk_hash, chk_labels[i]));
    else if (app.task == "ner")
      printf("\t%10s", SENNA_Hash_key(ner_hash, ner_labels[i]));
    printf("\n");
  }
  // end of sentence
  printf("\n");

    for( int i=0; i<4; i++){
    switch(i){
    case 0:
        LOG(ERROR) << "PRE Time(ns): " << uint64_to_string(times[i]) << endl;
        break;
    case 1:
        LOG(ERROR) << "DATA Time(ns): " << uint64_to_string(times[i]) << endl;
        break;

    case 2:
        LOG(ERROR) << "SEND Time(ns): " << uint64_to_string(times[i]) << endl;
        break;

    case 3:
        LOG(ERROR) << "COM Time(ns): " << uint64_to_string(times[i]) << endl;
        break;

    default:
       printf("You are not supposed to see this!! \n");

        }
   }

  // clean up
  SENNA_Tokenizer_free(tokenizer);

  SENNA_POS_free(pos);
  SENNA_CHK_free(chk);
  SENNA_NER_free(ner);

  SENNA_Hash_free(word_hash);
  SENNA_Hash_free(caps_hash);
  SENNA_Hash_free(suff_hash);
  SENNA_Hash_free(gazt_hash);

  SENNA_Hash_free(gazl_hash);
  SENNA_Hash_free(gazm_hash);
  SENNA_Hash_free(gazo_hash);
  SENNA_Hash_free(gazp_hash);

  SENNA_Hash_free(pos_hash);

  SOCKET_close(app.socketfd, debug);

  if (!app.djinn) free(app.net);

  return 0;
}
