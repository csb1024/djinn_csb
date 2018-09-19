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
        pvec = np.array([num for num in vec if num >= 0]) 
	nvec=sorted(pvec)
	del nvec[-1] # exclude the outlier
	return np.percentile(nvec, 95)

def average(vec):
	pvec = np.array([num for num in vec if num >= 0]) 
	nvec=sorted(pvec)
	del nvec[0]
	del nvec[-1] # exclude the outliers
	return sum(nvec) / len(nvec)

def med(vec):
	pvec = vec[vec>=0]
	nvec=sorted(pvec)
	nlen=len(vec)
	if nlen ==0:
		return 0
	else:
		return np.percentile(nvec,50)
	
def parse_args():  
	parser = ArgumentParser(description=__doc__,  
			formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('log_root_dir',
			help='directory which holds log files')
	parser.add_argument('result_file',
			help='file which is going to hold analyzed results')
	args = parser.parse_args()
	return args

benchmarks=('imc', 'dig', 'face', 'ner', 'pos', 'chk')
#means=[0.1, 0.05, 0.01, 0.00125]

means=[0.01]
total_input=64

def parsetoFile( result_file, log_root_dir):
	data=[]
	with open(result_file,"w") as fout:
		fout.write("mean,mode,bench,avg(ms),tail(ms),makespan(ms)\n")
	for mean in means:
		for subdir in os.listdir(log_root_dir):
			if float(subdir) ==  mean: # check dir name matches interested batch size
				print "processing files under "+subdir
				for subsubdir in os.listdir(log_root_dir+"/"+subdir):
					mode=subsubdir
					for f in os.listdir(log_root_dir+"/"+subdir+"/"+subsubdir):
						file_type=f.split(".")[-1]
						if file_type == "csv":
							with open (log_root_dir+"/"+subdir+"/"+subsubdir+"/"+f) as fp:
								print "processing " + f
								lines = fp.readlines()
								perf_info = {}
								bench_cnt={}
								for bench in benchmarks:
									perf_info[bench] = []
									bench_cnt[bench] = 0
								cnt=8 # for skipping the first line and 6 warmup results + addition line for chk
								for line in lines:
									if cnt != 0:
										cnt = cnt - 1
										continue
									words=line.split(',')
									bench=words[0]
									performance=float(words[-3])
									bench_cnt[bench]=bench_cnt[bench] +1
									perf_info[bench].append(performance)
								for bench in benchmarks:
									makespan=0
									tail_latency = tail(perf_info[bench])
									avg_latency  = average(perf_info[bench])
									makespan = float(sum(perf_info[bench]) * float( 5000.0 / bench_cnt[bench]))								
									item=(mean,mode,bench,avg_latency,tail_latency,makespan)
									data.append(item)
#								fa.write(mode+","+bench+","+str(i)+","+str(avg_delay)+","+str(tail_latency)+"\n")
#								fa.write(mode+","+bench+","+str(batch)+","+str(latency)+"\n")
#								fa.close()
#					print "total number of tasks logged : " + str(max_tid+1-7)
	with open (result_file, "a") as fa:
#		for item in data:
#			for bench in benchmarks:
#				for batch in batches:
#					if item[2] == bench and item[3] == batch and item[2]
		sorted_data = sorted(data,key=itemgetter(0,1,2))
#		for bench in benchmarks:
#			for batch in batches:
#				fa.write(bench+","+str(batch)+",")
#				data_vec=[]
#				for item in sorted_data: 
#					if item[0] == bench and item[1] == batch:
#						
#						data_vec.append(float(item[-1]))
#				sorted_vec = sorted(data_vec)
#				min_val=min(sorted_vec)
#				max_val=max(sorted_vec)
#				med_val=np.percentile(sorted_vec,50)
#				q1=np.percentile(sorted_vec,25)
#				q3=np.percentile(sorted_vec,75)
#				fa.write(str(med_val)+","+str(q1)+","+str(min_val)+","+str(max_val)+","+str(q3)+"\n")
					
		for item in sorted_data:
			for i in xrange(len(item)):
				fa.write(str(item[i])+",")
			fa.write("\n")

						
def main():
	args = parse_args()
	parameter_file="param.txt" # the file which holds model related parameters
	parsetoFile(args.result_file, args.log_root_dir)

if __name__ == '__main__':
	main()

