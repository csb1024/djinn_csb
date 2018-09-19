#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir

def average(numbers):
        total = sum(numbers)
        total = float(total)
        return total / len(numbers)


def parseLogsforTime(log_dir,output_file):
	with open(output_file,"w") as fout:
                fout.write("Batch,APP,max_util(%),max_mem_usage(%),max_pcie_util(%)\n")
        	for f in listdir(log_dir):
			print f 
                	tokens=f.split("-")
                	batch = tokens[0]
                	app = tokens[1]			
                	nums=[]
			nums2=[]
			nums3=[]
			isFirst=True
                	with open(log_dir+"/"+f,"r") as fp:
				lines = fp.readlines()
				lines=lines[:-1]
                        	for line in lines:
					if isFirst:
						isFirst=False # for excluding the first line of logs
						continue
                                        util=float(line.split(",")[-2])
					mem=float(line.split(",")[1])
					pcie_tx=float(line.split(",")[2])
					pcie_rx=float(line.split(",")[3])						
                                        nums.append(util)
					nums2.append(mem)
					nums3.append(pcie_tx)
					nums3.append(pcie_rx)
                        max_util=max(nums)
			max_mem=max(nums2)
			max_pci=max(nums3)
			fout.write(batch+","+app+","+str(max_util)+","+str(max_mem)+","+str(max_pci)+"\n")
                        

      
# leaving the argument function and main function for debugging
def parse_args():
        parser = ArgumentParser(description=__doc__,
                        formatter_class=ArgumentDefaultsHelpFormatter)
        parser.add_argument('log_dir',
                        help='dir which holds nvml log')
        parser.add_argument('output_file',
                        help='file to store output')
        args = parser.parse_args()
        return args

def main():
        args = parse_args()
        parseLogsforTime(args.log_dir,args.output_file)
if __name__ == '__main__':
        main()


