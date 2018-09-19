#!/usr/bin/env python
import sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from os import listdir

"""
** Metric was monitored in the following order 
1. dram_utilization -> line num 7
2. achieved_occupancy -> line num 8
3. sysmem_utilization -> line num 9
4. executed_ipc -> line num 10
"""

"""
**
Execution time was added to this code,2017-4-12
"""

def parseList(input_list ,output_list):
	#please change the following for different directories
	dir_str="input/dig/"
	with open(output_list,"w") as fout:
		with open(input_list,"r") as fp:
			for line in fp:
				new_file=dir_str+line
				fout.write(new_file)
		

# leaving the argument function and main function for debugging
def parse_args():
	parser = ArgumentParser(description=__doc__,
			formatter_class=ArgumentDefaultsHelpFormatter)
	parser.add_argument('input_file',
			help='input list file')
	parser.add_argument('output_file',
			help='file to store output list')
	args = parser.parse_args()
	return args
def main():
	args = parse_args()
	parseList(args.input_file,args.output_file)
if __name__ == '__main__':
	main()
