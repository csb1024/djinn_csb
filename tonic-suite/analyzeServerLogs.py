from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter 
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score, median_absolute_error
import numpy as np
import pandas as pd
import csv 
import os  


def tail(vec):
	nvec=sorted(vec)
	del nvec[-1] # exclude the outlier
	return nvec[-1]


def average(vec):
	nvec=sorted(vec)
	del nvec[0]
	del nvec[-1] # exclude the outliers
	return sum(nvec) / len(nvec)
	
def parse_args():  
	parser = ArgumentParser(description=__doc__,  
			formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('log_dir',
			help='directory which holds log files')
	parser.add_argument('result_file',
			help='file which is going to hold analyzed results')
	args = parser.parse_args()
	return args

#batchs=('overfeat')
benchmarks=('imc', 'dig', 'face')
#batchs=('vgg_16', 'vgg_19')#batchs to analyze


def savetoFile(batch_max, result_file, log_dir):
	# iterations, 
	iterations=20
	with open(result_file,"w") as fout:
#		fout.write("benchmark,b_over_int,latency\n")
		fout.write("benchmark,batch,interference,latency\n")
		profiled_info = {}			
		for bench in benchmarks:
			profiled_info[bench]=[]
			for batch in range(batch_max):
				new=[]
				for x in range(11):
					new.append(0)
				profiled_info[bench].append(new)
		for f in os.listdir(log_dir):
			print "Processing "+f
			batch=int(f[:-4])
			cnt = 1 # needed for skipping the first line
			with open (log_dir+"/"+f,"r") as fp: 
				lines = fp.readlines()
				perf_vec = []
				for line in lines:
					if cnt == 1:
						cnt = cnt + 1
						continue
					words=line.split(',')
					bench=words[0]
					tid=int(words[1])
					performance=float(words[-3])
					perf_vec.append(performance)
					if len(perf_vec) == iterations:
						avail=((tid%(iterations*11))/iterations)*10
#						print avail
#						profiled_info[bench][batch-1][avail/10]=average(perf_vec)
						profiled_info[bench][batch-1][avail/10]=tail(perf_vec)

						perf_vec=[]

		for bench in benchmarks:
			for batch in range(1,batch_max+1):
				for avail in range(0,11):
					# avoid devide-by-zero 
#					if avail == 10:
#						b_over_int=float(batch*20)
#					else:
#						b_over_int=float(batch/(float(10-avail)/10))
#					fout.write(bench+","+str(b_over_int)+","+str(profiled_info[bench][batch-1][avail])+"\n")
					fout.write(bench+","+str(batch)+","+str(avail*10)+","+str(profiled_info[bench][batch-1][avail])+"\n")
		fout.close()

def doLinearRegression(input_file,param_file):
	df = pd.read_csv(input_file)
#	print whole_X.head()
	print "evaluating linear model"
	for bench in benchmarks:
		print bench
		bench_df = df.loc[df['benchmark'] == bench]
		X=bench_df.drop('latency', axis=1)
		X=X.drop('benchmark', axis=1)
		lm = LinearRegression(fit_intercept=True)
		y_true=bench_df['latency']
		normalized_y_true=bench_df['latency'] / max(bench_df['latency']) 
		lm.fit(X,normalized_y_true)
		y_pred=lm.predict(X) * max(bench_df['latency'])
		print "mean squared error: "+ str(mean_squared_error(y_true,y_pred))
		print "median absolute error: "+str(median_absolute_error(y_true, y_pred))
		print "r2 score: "+str(r2_score(y_true, y_pred))

def doLogModelFitting(input_file,param_file):
	df = pd.read_csv(input_file)
	print "evaluating log model"
	for bench in benchmarks:
		print bench
		bench_df = df.loc[df['benchmark'] == bench]
		X=bench_df.b_over_int
		y_true=bench_df.latency / max(bench_df.latency)
		param=np.polyfit(np.log(X),y_true,1)
		y_pred=param[0]*np.log(X)+param[1]
		print "mean squared error: "+ str(mean_squared_error(y_true,y_pred))
		print "median absolute error: "+str(median_absolute_error(y_true, y_pred))
		print "r2 score: "+str(r2_score(y_true, y_pred))


def main():
	args = parse_args()
	batch_max = 8
	parameter_file="param.txt" # the file which holds model related parameters
	savetoFile(batch_max, args.result_file, args.log_dir)
	doLinearRegression(args.result_file, parameter_file)
#	doLogModelFitting(args.result_file, args.log_dir)

if __name__ == '__main__':
	main()

