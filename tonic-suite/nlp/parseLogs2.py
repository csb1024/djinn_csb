#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir


def average(numbers):
	total = sum(numbers)
	total = float(total)
	return total / len(numbers)



def parseLogsforTime(log_dir,output_file):
	ner_vec = []
	for i in xrange(20):
 		ner_vec.append(0)
 	pos_vec = []
	for i in xrange(20):
		pos_vec.append(0)
	chk_vec = []
	for i in xrange(20):
		chk_vec.append(0)
	for f in listdir(log_dir):
		tokens=f.split("-")
		batch = tokens[0]
 		app = tokens[1]
		nums=[]
		with open(log_dir+"/"+f,"r") as fp:
			for line in fp:
				if "Time(ns): " in line:
					print line
					ms=int(line.split(" ")[-1])
					nums.append(ms)
				continue
			if app == "ner":
				ner_vec[int(batch)-1]=float(max(nums)) / 1000000
			elif app == "pos":
				pos_vec[int(batch)-1]=float(max(nums)) / 1000000
			elif app == "chk":
				chk_vec[int(batch)-1]=float(max(nums)) / 1000000
		
	with open(output_file,"w") as fout:
		fout.write("batch,NER,POS,CHK\n")
		for i in xrange(0,20):			
			fout.write(str(i+1)+","+str(ner_vec[i])+","+str(pos_vec[i])+","+str(chk_vec[i])+"\n")
		
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
