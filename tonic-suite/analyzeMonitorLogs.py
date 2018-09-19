from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter 

import numpy as np
import csv
import os 
import itertools
import operator

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
		fout.write("Workload,Exec Time(s),Peak DRAM Occupancy(%),AVG Comp Util(%),AVG Mem Util(%),AVG PCIe Util(%)\n")
	for i in range(1,iterations+1):
		for subdir in os.listdir(log_root_dir):
			iteration = subdir.split('-')[0]
			avg_delay = subdir.split('-')[1]
		       	if int(iteration) ==  i:  
				for f in os.listdir(log_root_dir+"/"+subdir):											
					print "processing files under "+subdir	
					with open (log_root_dir+"/"+subdir+"/"+f) as fp: 
						print "processing " + f 

		for f in os.listdir(args.log_dir):
			with open(args.log_dir+"/"+f,"r") as fp:
				#extract title of file, "   "-monitor_log.txt
				title=f[:-16]
				print "Processing"+title
				# flags required for a file
				first_line = True
				
				lines = fp.readlines()

				# process last line, contains exec time
				last_line = lines[-1]					
				last_words = last_line.split()
				exec_time = float(last_words[-1])					
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
					pci_tx.append(float(words[2]))
					pci_rx.append(float(words[3]))
					core_util.append(float(words[4]))
					mem_util.append(float(words[5]))
				
				
				max_mem_usage = most_common(mem_usage)
				sum_pci_tx = 0.
				sum_pci_rx = 0.
				sum_core_util = 0.
				sum_mem_util = 0.
				cnt = 0
				for i in range(len(mem_usage)):
					if mem_usage[i] == max_mem_usage:
						cnt = cnt + 1
						sum_pci_tx = sum_pci_tx + pci_tx[i]
						sum_pci_rx = sum_pci_rx + pci_rx[i]
						sum_core_util = sum_core_util + core_util[i]
						sum_mem_util = sum_mem_util + mem_util[i]
				
				avg_pci_util = (sum_pci_tx + sum_pci_rx) /( 2*cnt) 
				avg_core_util = sum_core_util / cnt
				avg_mem_util = sum_mem_util / cnt
				fout.write(title+","+str(exec_time)+","+str(max_mem_usage)+","+str(avg_core_util)+","+str(avg_mem_util)+","+str(avg_pci_util)+"\n")
	
						
			


if __name__ == '__main__':
	main()
