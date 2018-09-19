import thread
import time
import numpy as np 
import subprocess
import os
# COMMON 
PORT='8080'
COMMON="/home/sbchoi/org/djinn-1.0/common"

#EXP related parameters 
NTHREAD=2 #number of concurrent threads calling DNN services
LAMBDA=4 # lambda value represents how 'frequent' requests are. smaller =  more frequent  

#np.random.exponential(LAMBDA/NTHREAD, 1)
#np.random.randint(0, 6)

def callImc_asynch(batch_size): #batch_size : int
#  os.chdir(os.getcwd()+"/img")
	p=subprocess.Popen(['./tonic-img','--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list3.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
#  p.wait()
	return p
def callImc_synch(batch_size): #batch_size : int
#  os.chdir(os.getcwd()+"/img")
	p=subprocess.Popen(['./tonic-img','--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list3.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
	p.wait()
	return


def average(numbers):
        total = sum(numbers)
        total = float(total)
        return total / len(numbers)

  
def main():
  	batch_size =1
	with open("time_results","w") as results:
		results.write("back-to-back,concurrent,batching\n")
		nums=[]
#		nums=[15]
		for i in range(2,16):
			nums.append(i)

		time_vec = [] 
		for num in nums:

			times=[]
#			print str(num) 
			for j in xrange(5):
				procs= []
				start=time.time()
				callImc_synch(num)

				end=time.time()
				times.append(end-start)
#			print str(len(times))
			avg=average(times)
			print str(avg)
			time_vec.append(avg)
#			time.sleep(1)
#			start=time.time()
#			for i in xrange(num):
#				procs.append(callImc_asynch(batch_size))
#			for i in xrange(num):
#				procs[i].wait()
				
#			end=time.time()
#			time.sleep(1)
#			time_vec.append(end-start)
#			start=time.time()
#			callImc_synch(num)
#			end=time.time()
#			time.sleep(1)
#			time_vec.append(end-start)
#			results.write(str(num)+",")
			for i in xrange(len(time_vec)):
				results.write(str(time_vec[i])+"\n")
	#		results.write("\n")
if __name__  == '__main__' :
        main()

