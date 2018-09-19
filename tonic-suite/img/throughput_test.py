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




def callImc(batch_size): #batch_size : int

#  os.chdir(os.getcwd()+"/img")
  p=subprocess.Popen(['./tonic-img','--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list3.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  p.wait()
  return
  
def main():
	time_vec = [] # vector that stores latency info
  	batch_vec = [1,2,4,8,16,32,64,128,256,512,1024]
#	batch_vec = [128,256]
 
#org_path=os.getcwd()
	for batch_size in batch_vec:
		epoch=(4096/batch_size)
		start=time.time()
		for i in xrange(epoch):
        		callImc(batch_size)
        		print i
		end=time.time()
		print str(end-start)
		time_vec.append(end-start)
	with open("time_results","w") as results:
	
		for i in xrange(len(batch_vec)):
			results.write(str(batch_vec[i])+","+str(time_vec[i])+"\n")
#    interval=np.random.exponential(LAMBDA/NTHREAD, 1)    
#    print interval
 #   time.sleep(interval)
if __name__  == '__main__' :
        main()

