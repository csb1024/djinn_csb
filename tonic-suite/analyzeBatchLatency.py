from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter 
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score, median_absolute_error
import numpy as np
import pandas as pd
import csv 
import os  
from operator import itemgetter


def tail(vec):
	if len(vec) == 0:
		return 0
	elif len(vec) == 1:
		return vec[0]
	nvec=sorted(vec)
	del nvec[-1] # exclude the outlier
	return nvec[-1]

def average(vec):
	if len(vec) == 0:
		return 0
	if len(vec) == 1 or len(vec) ==2:
		return vec[0]
	nvec=sorted(vec)
	del nvec[0]
	del nvec[-1] # exclude the outliers
	return sum(nvec) / len(nvec)

def med(vec):
	nvec=sorted(vec)
	nlen=len(vec)
	if nlen ==0:
		return 0
	else:
		return nvec[nlen/2]
	
def parse_args():  
	parser = ArgumentParser(description=__doc__,  
			formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('log_root_dir',
			help='directory which holds log files')
	parser.add_argument('result_file',
			help='file which is going to hold analyzed results')
	args = parser.parse_args()
	return args

#benchmarks=('imc', 'dig', 'face', 'ner', 'pos', 'chk')
benchmarks=('dig', 'ner', 'pos', 'chk')


def parsetoFile( result_file, log_root_dir):
	# iterations, 
	batches=32
	data = []
	with open(result_file,"w") as fout:
		fout.write("mode,benchmark,batch,latency\n")
	for i in range(1,batches+1):
		for subdir in os.listdir(log_root_dir):
			if int(subdir) ==  i: # check dir name matches interested batch size
				for f in os.listdir(log_root_dir+"/"+subdir):
					print "processing files under "+subdir
					file_type=f.split(".")[-1]
					if file_type == "csv":
						with open (log_root_dir+"/"+subdir+"/"+f) as fp:
							print "processing " + f
							mode =f[:-4]
							lines = fp.readlines()
							perf_info = {}
							for bench in benchmarks:
								perf_info[bench] = []
#							cnt = len(benchmarks)+1 # for skipping the first line and warmup results for each benchmark
#							if 'chk' in benchmarks:
#								cnt=cnt +1 # chk requests pos internally, so add another line to skip
							cnt =1 
							for line in lines:
								if cnt != 0:
									cnt = cnt - 1
									continue
								words=line.split(',')
								bench=words[0]
								performance=float(words[-3])
								perf_info[bench].append(performance)
			
							for bench in benchmarks:
#								latency = tail(perf_info[bench])
								latency = average(perf_info[bench])
								item=(mode, bench, i, latency)
								data.append(item)
	with open (result_file, "a") as fa:
		sorted_data = sorted(data,key=itemgetter(0,1,2))
		for item in sorted_data:
			fa.write(str(item[0])+","+str(item[1])+","+str(item[2])+","+str(item[3])+"\n")



						
def main():
	args = parse_args()
	parsetoFile(args.result_file, args.log_root_dir)
	


if __name__ == '__main__':
	main()

