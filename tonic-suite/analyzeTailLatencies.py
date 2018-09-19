from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter 
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score, median_absolute_error
import numpy as np
import pandas as pd
import csv 
import os  


def tail(vec):
	if len(vec) == 0:
		return 0
	elif len(vec) == 1:
		return vec[0]
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
	parser.add_argument('log_root_dir',
			help='directory which holds log files')
	parser.add_argument('result_file',
			help='file which is going to hold analyzed results')
	args = parser.parse_args()
	return args

benchmarks=('imc', 'dig', 'face', 'ner')

def parsetoFile( result_file, log_root_dir):
	# iterations, 
	iterations=10
	with open(result_file,"w") as fout:
		fout.write("avg-delay,mode,benchmark,iteration,tail-latency\n")
	for i in range(1,iterations+1):
		for subdir in os.listdir(log_root_dir):
			iteration = subdir.split('-')[0]
			avg_delay = subdir.split('-')[1]
			if int(iteration) ==  i:
				for f in os.listdir(log_root_dir+"/"+subdir):
					print "processing files under "+subdir
					with open (log_root_dir+"/"+subdir+"/"+f) as fp:
						print "processing " + f
						mode =f[:-4]
						lines = fp.readlines()
						perf_info = {}
						for bench in benchmarks:
							perf_info[bench] = []										
						cnt=5 # for skipping the first line and 4 warmup results
						for line in lines:
							if cnt != 0:
								cnt = cnt - 1
								continue
							words=line.split(',')
							bench=words[0]
							performance=float(words[-3])
							perf_info[bench].append(performance)
			
						with open (result_file, "a") as fa:
							for bench in benchmarks:
								tail_latency = tail(perf_info[bench])
#								fa.write(mode+","+bench+","+str(i)+","+str(avg_delay)+","+str(tail_latency)+"\n")
								fa.write(str(avg_delay)+","+mode+","+bench+","+str(i)+","+str(tail_latency)+"\n")

#								fa.close()


						



def main():
	args = parse_args()
	batch_max = 8
	parameter_file="param.txt" # the file which holds model related parameters
	parsetoFile(args.result_file, args.log_root_dir)

if __name__ == '__main__':
	main()

