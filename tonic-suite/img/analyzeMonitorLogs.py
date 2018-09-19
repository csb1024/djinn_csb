from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter 

import numpy as np
import csv
import os 
import itertools
import operator

def average(numbers):
	total = sum(numbers)
	total = float(total)
	return total / len(numbers)

def most_common(L):
  # get an iterable of (item, iterable) pairs
  SL = sorted((x, i) for i, x in enumerate(L))
  # print 'SL:', SL
  groups = itertools.groupby(SL, key=operator.itemgetter(0))
  # auxiliary function to get "quality" for an item
  def _auxfun(g):
    item, iterable = g
    count = 0
    min_index = len(L)
    for _, where in iterable:
      count += 1
      min_index = min(min_index, where)
    # print 'item %r, count %r, minind %r' % (item, count, min_index)
    return count, -min_index
  # pick the highest-count/earliest item
  return max(groups, key=_auxfun)[0]
def parse_args():  
	parser = ArgumentParser(description=__doc__,  
		formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('log_dir',
		help='directory which holds monitor log files')
	parser.add_argument('result_file',
		help='file which is going to hold analyzed results')
	args = parser.parse_args()
	return args

def main():
	args = parse_args()
	with open(args.result_file,"w") as fout:
		fout.write("Workload,batch size,Peak DRAM Occupancy(%),MAX Comp Util(%),MAX Mem Util(%),AVG PCIe Util(%)\n")
		for f in os.listdir(args.log_dir):
			with open(args.log_dir+"/"+f,"r") as fp:
				#extract title of file : "batch"-workload"-log
				keywords=f.split("-")
				batch=keywords[0]
				workload=keywords[1]
				print "Processing "+workload +" of batch size  " +batch
				# flags required for a file
				first_line = True
				lines = fp.readlines()
				# process last line, contains exec time
				#last_line = lines[-1]					
				#last_words = last_line.split()
				#exec_time = float(last_words[-1])					
				del lines[-1]
				
				mem_usage = []
				pci_tx = []
				pci_rx = []
				core_util = []
				mem_util = []

				# read every info line by line
				for line in lines:
					if first_line:
						# skip first line
						first_line = False
						continue
					words = line.split(',')					
					mem_usage.append(float(words[1]))
					if float(words[2]) != 0.0:
						pci_tx.append(float(words[2]))
					if float(words[3]) != 0.0:
						pci_rx.append(float(words[3]))
					core_util.append(float(words[4]))
					mem_util.append(float(words[5]))
				
				# most common 
				max_mem_usage = most_common(mem_usage)
				max_core_util = max(core_util)
				avg_pci_tx =average(pci_tx) 
				avg_pci_rx =average(pci_rx)
				max_mem_util = max(mem_util)
				avg_pci_util = (avg_pci_tx + avg_pci_rx) / 2
				fout.write(workload+","+batch+","+str(max_mem_usage)+","+str(max_core_util)+","+str(max_mem_util)+","+str(avg_pci_util)+"\n")

if __name__ == '__main__':
	main()
