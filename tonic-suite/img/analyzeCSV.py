#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir


class profInfo:
	def __init__(self,batch,time)
		self.batch_size=batch
		self.time=time

		
		
def average(numbers):
	total = sum(numbers)
	total = float(total)
	return total / len(numbers)



def parseLogs(log_dir,output_file, applist,N): # N : number of test cases 
	with open(output_file,"w") as fout:	
		fout.write("batch,APP,COM\n")
	img_dict = {} # key : string(pid) value : profInfo class

	for app in applist:
		img_dict[app] = [{} for y in range(N)]
	for app in applist:
		print "Processing "+app
	for f in listdir(log_dir):
		print "Processing "+f
			timer=1
			with open(log_dir+"/"+f,"r") as fp:
				for line in fp:
					if timer >0:
						timer = timer = -1
						continue
					words = line.split(",")	
					if words[0] != app:
						continue 
					batch=int(words[1])
					value=float(words[-1])
					print words[-1]
					img_dict[app][batch]=value
	with open(output_file, "a") as fout:			
		for app in img_dict:
			for i in range(N):
				med_value=getMedian(img_dict[app][i])
				fout.write(app+","+str(i+1)+","+str(med_value)+"\n")

def getMedian(nums):
	if len(nums) == 0:
		return 0
	else:
		sorted_nums = sorted(nums)
		return sorted_nums[len(nums)/2]
def average(nums):
	if len(nums) == 0:
		return 0
	else:
	 	return sum(nums)/len(nums)

# leaving the argument function and main function for debugging
def parse_args():
	parser = ArgumentParser(description=__doc__,
			formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('csv_dir',
			help='directory which holds the csv files')
	parser.add_argument('output_file',
			help='file to store output vector')
	args = parser.parse_args()
	return args
def main():
	args = parse_args()
	# the following are parameters for analyzing results 
	applist=("imc", "dig","face")
	N = len(listdir(args.csv_dir))
	parseLogs(args.csv_dir,args.output_file,applist,N)
if __name__ == '__main__':
	main()
