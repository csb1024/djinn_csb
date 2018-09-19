#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir
from numpy import median

def average(numbers):
	total = sum(numbers)
	total = float(total)
	return total / len(numbers)




def parseLogsforTime(log_dir,output_file):
	pos_batch_vec=[]
	ner_batch_vec=[]
	chk_batch_vec=[]
	pos_vec = []
 	ner_vec = []
	chk_vec = []
	for f in listdir(log_dir):
		tokens=f.split("-")
		batch = tokens[0]
 		app = tokens[1]
		PRE_nums=[]
		DATA_nums=[]
		SEND_nums=[]
		COM_nums=[]
		with open(log_dir+"/"+f,"r") as fp:
			for line in fp:
				if "Time(ns): " in line:
					print line
					ms=int(line.split(" ")[-1])
					if "PRE" in line:
						PRE_nums.append(ms)
					elif "DATA" in line:
						DATA_nums.append(ms)
					elif "SEND" in line:
						SEND_nums.append(ms)
					elif "COM" in line:
						COM_nums.append(ms)
				continue
			if app == "pos":
				pos_batch_vec.append(int(batch))
				pos_vec.append(median(PRE_nums) / 1000000)
				pos_vec.append(median(DATA_nums) / 1000000)
				pos_vec.append(median(SEND_nums) / 1000000)
				pos_vec.append(median(COM_nums) / 1000000)

			elif app == "ner":
				ner_batch_vec.append(int(batch))
				ner_vec.append(median(PRE_nums) / 1000000)
				ner_vec.append(median(DATA_nums) / 1000000)
				ner_vec.append(median(SEND_nums) / 1000000)
				ner_vec.append(median(COM_nums) / 1000000)
			elif app == "chk":
				chk_batch_vec.append(int(batch))
				chk_vec.append(median(PRE_nums) / 1000000)
				chk_vec.append(median(DATA_nums) / 1000000)
				chk_vec.append(median(SEND_nums) / 1000000)
				chk_vec.append(median(COM_nums) / 1000000)
	with open(output_file,"w") as fout:
		fout.write("-----CHK-----\n")
		fout.write("batch,PRE,DATA,SEND,COM\n")
		for i in xrange(len(chk_batch_vec)):
			fout.write(str(chk_batch_vec[i])+","+str(chk_vec[i*4])+","+str(chk_vec[i*4+1])+","+str(chk_vec[i*4+2])+","+str(chk_vec[i*4+3])+"\n")
		fout.write("-----NER-----\n")
		fout.write("batch,PRE,DATA,SEND,COM\n")
		for i in xrange(len(ner_batch_vec)):
			fout.write(str(ner_batch_vec[i])+","+str(ner_vec[i*4])+","+str(ner_vec[i*4+1])+","+str(ner_vec[i*4+2])+","+str(ner_vec[i*4+3])+"\n")
		fout.write("-----POS-----\n")
		fout.write("batch,PRE,DATA,SEND,COM\n")
		for i in xrange(len(pos_batch_vec)):
			fout.write(str(pos_batch_vec[i])+","+str(pos_vec[i*4])+","+str(pos_vec[i*4+1])+","+str(pos_vec[i*4+2])+","+str(pos_vec[i*4+3])+"\n")



		
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
