#include "client_model.h"
#include <random>

client_model::client_model(int nclient, double lambda){
	_nclient = nclient;
	_lambda = lambda;
}

client_model::~client_model(){
}


double client_model::getRandNumber(){
	random_device gen;
	exponential_distribution<double> exp_dist(_nclient * (_lambda));
	return exp_dist(gen);

}


double client_model::randomExponentialInterval(double mean) {
	  static std::mt19937* rng = new std::mt19937();
	    std::uniform_real_distribution<double> dist(0, 1.0);
	      /* Cap the lower end so that we don't return infinity */
	      return - log(std::max(dist(*rng), 1e-9)) * mean;
}

