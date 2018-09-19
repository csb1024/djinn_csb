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
  subprocess.Popen(['./tonic-img','--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False,cwd=os.getcwd()+"/img")
  return
  
  
def callFace(batch_size):
  IMC="img/tonic-img"
  subprocess.popen([IMC,'--common',COMMON,'--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  return

def callDig(batch_size):
  IMC="img/tonic-img"
  subprocess.popen([IMC,'--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  return

def callAsr(batch_size):
  IMC="img/tonic-img"
  subprocess.popen([IMC,'--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  return

def callPos(batch_size):
  IMC="img/tonic-img"
  subprocess.popen([IMC,'--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  return

def callNer(batch_size):
  IMC="img/tonic-img"
  subprocess.popen([IMC,'--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  return

def callChk(batch_size):
  IMC="img/tonic-img"
  subprocess.popen([IMC,'--task','imc','--network','imc.prototxt','--weight', 'imc.caffemodel','--input','imc-list2.txt', '--djinn', '1','--portno', PORT,'--batch',str(batch_size)],shell=False)
  return

def main():
#org_path=os.getcwd()
  for i in xrange(10):
    callImc(1)
    interval=np.random.exponential(LAMBDA/NTHREAD, 1)    
    print interval
    time.sleep(interval)
if __name__  == '__main__' :
        main()

