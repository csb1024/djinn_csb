from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter 

import numpy as np
import csv
import os 

def parse_args():  
	parser = ArgumentParser(description=__doc__,  
		formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('log_dir',
		help='directory which holds monitor log files')
	parser.add_argument('result_file',
		help='file which is going to hold analyzed results')
	args = parser.parse_args()
	return args

#models=('overfeat')
models=('alexnet', 'lenet', 'vgg_cnn_s', 'overfeat','squeezenet')
#models=('vgg_16', 'vgg_19')#models to analyze
def main():
	args = parse_args()
	batch_max = 200
	with open(args.result_file,"w") as fout:
		fout.write("Workload,95%-ile comp util(%)\n")
		for model in models:
			vector_2=[] #batch
			
			for batch in range(1,batch_max +1):								
				vector_3=[] # iteration
				for f in os.listdir(args.log_dir):
					title=f[:-5]
					
					model_name=title.split('-')[0]
					batch_size=title.split('-')[1]
					if  model_name == model and batch_size == str(batch):
						#print "model: "+model_name+" batch_size:  "+batch_size

						print "Processing "+f
						with open (args.log_dir+"/"+f,"r") as fp:
							lines = fp.readlines()
							exec_time=lines[0] # files has only one number, one line
						vector_3.append(float(exec_time))
				# sort time in acsending order
				vector_3.sort()
				vector_2.append(vector_3[int(len(vector_3)*0.95)]) # 19th element among 20
			for i in range(1, batch_max+1):
				string=model+"-"+str(i)+","+str(vector_2[i-1])+"\n"
				fout.write(string)	

if __name__ == '__main__':
	main()
