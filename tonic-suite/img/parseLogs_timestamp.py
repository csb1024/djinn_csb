#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir
from numpy import median 

class procInfo:
	def __init__(self,batch,pid,app): # batch:string, pid:string , app:string
		self.batch_size=batch # batch size
		self.pid = pid
		self.app = app
		self.times = {} # Time point related info, string,long,pair

def parseLogsforTime(log_dir,output_file):
	with open(output_file,"w") as fout:	
		fout.write("app,batch,PID,PRE,DATA,SEND,COM\n")

	for f in listdir(log_dir):
		img_dict = {} # key : string(pid) value : profInfo class
		tokens=f.split("-")
		batch = tokens[0]
 		app = tokens[1]
		start_time=0
		bar_time=0
		pre_time=0
		data_time=0
		send_time=0
		com_time=0
		with open(log_dir+"/"+f,"r") as fp:
			for line in fp:
				if "Time(ns): " in line:
					ms=long(line.split(" ")[-2])
					pid=line.split(" ")[-1][:-1] # erase \n 
					if pid not in img_dict:
						img_dict[pid]=procInfo(batch,pid,app)
					if "START" in line:
						start_time=ms			
					elif "BARRIER" in line:
						bar_time=ms			
					elif "PRE" in line:
						pre_time=ms
					elif "DATA" in line:
						data_time=ms
					elif "SEND" in line:
						send_time=ms
					elif "COM" in line:
						com_time=ms
						img_dict[pid].times["PRE"]=float(pre_time - start_time)/1000000
						img_dict[pid].times["DATA"]=float(data_time - bar_time)/1000000
						img_dict[pid].times["SEND"]=float(send_time - data_time)/1000000
						img_dict[pid].times["COM"]=float(com_time - send_time)/1000000
 				continue
	
			
		with open(output_file, "a") as fout:			
			##get the middle value and print 
			timer=len(img_dict)/2
			COM_nums=[]
			for pid in img_dict:
				COM_nums.append(img_dict[pid].times["COM"])
			med_com_time = median(COM_nums)
			fout.write(app+","+batch+","+str(med_com_time)+"\n")
		#fout.write("batch,PRE,DATA,SEND,COM\n")
	#	return self.app+","+self.batch_size+","+self.pid+","+str(self.times["PRE"])+","+str(self.times["DATA"])+","+str(self.times["SEND"])+","+str(self.times["COM"])


		
# leaving the argument function and main function for debugging
def parse_args():
	parser = ArgumentParser(description=__doc__,
			formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('log_dir',
			help='input caffe log file')
	parser.add_argument('output_file',
			help='file to store output vector')
	args = parser.parse_args()
	return args
def main():
	args = parse_args()
	parseLogsforTime(args.log_dir,args.output_file)
if __name__ == '__main__':
	main()
