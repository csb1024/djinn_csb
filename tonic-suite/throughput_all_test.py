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



def callImc(batch_size, sync): #batch_size : int, sync : bool(1:sync,0:async)
	p=subprocess.Popen(['./tonic-img','--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list3.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/img")
	if sync:
		p.wait()
		return
	return p


def callFace(batch_size,sync):
	p=subprocess.Popen(['./tonic-img','--task','face','--network','face.prototxt','--weight', 'face.caffemodel','--input','face-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/img")
	if sync:
		p.wait()
		return
	return p

def callDig(batch_size, sync):
	p=subprocess.Popen(['./tonic-img','--task','dig','--network','dig.prototxt','--weight', 'dig.caffemodel','--input','dig-list3.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/img")
	if sync:
		p.wait()
		return
	return p


def callPos(batch_size,sync):
	p=subprocess.Popen(['./tonic-nlp','--task','pos','--network','pos.prototxt','--weight', 'pos.caffemodel','--input','input/sanity-test-input.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/nlp")
	return

def callNer(batch_size,sync):
	p=subprocess.Popen(['./tonic-nlp','--task','ner','--network','ner.prototxt','--weight', 'ner.caffemodel','--input','input/sanity-test-input.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/nlp")
	if sync: 
		p.wait()
		return
	return

def callChk(batch_size,sync):
	p=subprocess.Popen(['./tonic-nlp','--task','chk','--network','chk.prototxt','--weight', 'chk.caffemodel','--input','input/sanity-test-input.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/nlp")
	if sync:
		p.wait()
		return
	return p


#def callWorkload(batch_size,workload,batch_vec,total_input)
#	time_vec = []
#	epoch=(total_input/batch_size)
#	start=time.time()
#	for i in xrange(epoch):
#		callImc(batch_size,True)
#	end=time.time()
#	time_vec.append(end-start)
#	return time_vec
callfunc = {}
callfunc["dig"]=callDig
callfunc["imc"]=callImc
callfunc["face"]=callFace
callfunc["pos"]=callPos
callfunc["ner"]=callNer
callfunc["chk"]=callChk
def main():
	time_vec = [] # vector that stores latency info
#	workloads = ["imc", "dig", "face", "pos", "ner", "chk", "asr"]
	workloads = [ "asr"]
  	large_batch_vec = [1,2,4,8,16,32,64,128,256,512,1024]
	med_batch_vec = [1,2,4,8,16,32,64]
	nlp_batch_vec = [1,2,4,8,16,32,64,128,256,512]
	small_batch_vec = [1,2,4,8] 
#	batch_vec = [128,256]
 
#org_path=os.getcwd()
	with open("time_results","w") as results:
		for workload in workloads:
			if workload == "imc" or workload == "dig":
				batch_vec = large_batch_vec
				total_input = 4096
			elif workload == "face":
				batch_vec = med_batch_vec 
				total_input=64
			elif workload == "pos" or workload == "ner" or workload == "chk":
				batch_vec = large_batch_vec
				total_input = 1024
			else: # asr
				batch_vec = small_batch_vec
				total_input = 8
			for batch_size in batch_vec:
				epoch=(total_input/batch_size)
				start=time.time()
				for i in xrange(epoch):
					callfunc[workload](batch_size, True)
				end=time.time()
				time_vec.append(end-start)
			results.write("----"+workload+"----"+"\n")
			for i in xrange(len(batch_vec)):
				results.write(str(batch_vec[i])+","+str(time_vec[i])+"\n")
if __name__  == '__main__' :
        main()
