#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir

class procInfo:
	def __init__(self,batch,pid,app): # batch:string, pid:string , app:string
		self.batch_size=batch # batch size
		self.pid = pid
		self.app = app
		self.times = {} # Time point related info, string,long,pair
		self.PRE_time = 0.0
		self.DATA_time = 0.0
		self.SEND_time = 0.0
		self.COM_time = 0.0
	def calcTimes(self): # returns nothing just internally calculates time
		self.PRE_time = float(self.times["PRE"]-self.times["START"] ) / 1000000
		self.DATA_time = float(self.times["DATA"] - self.times["BARRIER"])/1000000
		self.SEND_time = float(self.times["SEND"] - self.times["DATA"])/1000000
		self.COM_time = float(self.times["COM"] - self.times["SEND"])/1000000

	def printTimes(self): #returns string
		return self.app+","+self.batch_size+","+self.pid+","+str(self.PRE_time)+","+str(self.DATA_time)+","+str(self.SEND_time)+","+str(self.COM_time)
	

		
			
		
def average(numbers):
	total = sum(numbers)
	total = float(total)
	return total / len(numbers)



def parseLogsforTime(log_dir,output_file, operation):
	with open(output_file,"w") as fout:	
		fout.write("batch,APP,PID,PRE,DATA,SEND,COM\n")

	for f in listdir(log_dir):
		img_dict = {} # key : string(pid) value : profInfo class
		tokens=f.split("-")
		app = tokens[0]
 		batch = tokens[1]
                PRE_nums=[]
                DATA_nums=[]
                SEND_nums=[]
                COM_nums=[]
		timer=5
		with open(log_dir+"/"+f,"r") as fp:
			print "Processing app "+app+" batch : "+batch
			for line in fp:
				if "Time(ns): " in line:
					ms=long(line.split(" ")[-2])
					pid=line.split(" ")[-1][:-1] # erase \n 
					if pid not in img_dict:
						img_dict[pid]=procInfo(batch,pid,app)
					if "START" in line:
					#	if timer == 0:						
							img_dict[pid].times["START"]=ms
					elif "BARRIER" in line:
					#	if timer == 0:						
							img_dict[pid].times["BARRIER"]=ms
					elif "PRE" in line:
					#	if timer == 0:						
							img_dict[pid].times["PRE"]=ms
					elif "DATA" in line:
					#	if timer == 0:						
							img_dict[pid].times["DATA"]=ms
					elif "SEND" in line:
					#	if timer == 0:						
							img_dict[pid].times["SEND"]=ms
					elif "COM" in line:
					#	if timer == 0:						
							img_dict[pid].times["COM"]=ms 											
				continue
		time_vec = []
		for pid in img_dict:
			img_dict[pid].calcTimes()
			time_vec.append(img_dict[pid].COM_time)
			if operation == "avg":
				value=average(time_vec)
			elif operation == "med":
				value = getMedian(time_vec)
			elif operation == "max":
				value=max(time_vec)
			else:
				print "please use avg OR med OR max"	
		with open(output_file, "a") as fout:			
			for pid in img_dict:
				if img_dict[pid].COM_time == value:
					fout.write(img_dict[pid].printTimes()+"\n")
					break

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
	parser.add_argument('log_dir',
			help='input caffe log file')
	parser.add_argument('output_file',
			help='file to store output vector')
	parser.add_argument('ops',
			help='statistical operation : avg OR med OR max')
	args = parser.parse_args()
	return args
def main():
	args = parse_args()
	parseLogsforTime(args.log_dir,args.output_file, args.ops)
if __name__ == '__main__':
	main()
