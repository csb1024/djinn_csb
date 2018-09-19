
#!/bin/bash

# DIRS
ROOT_DIR=$HOME/org/djinn-1.0
SERVER_DIR=$ROOT_DIR/service
CLIENT_DIR=$ROOT_DIR/tonic-suite
MON_DIR=$HOME/git/gpu-cloud/monitor/cpp3


#SERVER SCRIPTS
GPU_SERVER_SH=$SERVER_DIR/executeServer.sh
CPU_SERVER_SH=$SERVER_DIR/executeCPUServer.sh

#CLIENT SCRIPTS
IMG_SH=$CLIENT_DIR/img/execImg_Djinn_no_mpi.sh
NLP_SH=$CLIENT_DIR/nlp/execNlp_Djinn_no_mpi.sh

#MONITOR BINARY
MON_BIN=nvml_mon




#Experiment related variables
#APPS=('dig' 'imc' 'face' 'ner' 'chk' 'pos')
APPS=('dig' 'ner' 'chk' 'pos')
#APPS=('dig')
apps_num=${#APPS[@]}


BATCHES=`seq 1 20`
batch_num=${#BATCHES[@]}

#FLAGS
mon_flag=1 # flag for running monitor
flag_1=0 # CPU batch experiment
flag_2=1 # GPU utilization & batch experiment
flag_3=0 

#for ((i=0; i<${batch_num}; i++))
#do
#batch=${BATCHES[${i}]}
for batch in `seq 8 8`
do
RESULT_DIR=$CLIENT_DIR/results/$batch
mkdir -p $RESULT_DIR


####################
#1) first experiment : CPU BATCH PROFILE
####################
if [ $flag_1 -eq 1 ]
then

# 1 start server with 4 open blas threads 
	cd $SERVER_DIR
	$GPU_SERVER_SH cpu_only &

# give some time to startup
	sleep 15

	
# 2 warmup 
	echo "warmup for server"
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}

		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app 1 1 1
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 1 1
		fi
		sleep 1
	done
# 3 the real experimnet

	# run processes and store pids in array

	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}
	for iter in `seq 1 1`
	do
		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app 1 $batch 1 &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 $batch 1 &
		fi
		pids[${h}]=$!
		# wait for all pids
	for pid in ${pids[*]}; do
		wait $pid
	done # pid
	done # iter
	if [ $app == 'face' ] 
	then
		sleep 30
	else
		sleep 10
	fi

	done #app

# 4 stop server and clean up data 
	pkill djinn 
	mv $SERVER_DIR/log.txt $RESULT_DIR/cpu.csv
	# start monitor if flag is setted
fi


#####################
#2) second experiment : GPU utilization & batch experiment
#####################
if [ $flag_2 -eq 1 ]
then
#wait for previous server to fully shutdown
sleep 5

# start monitor if flag is setted
	


# 1. start GPU server 
	cd $SERVER_DIR
	$GPU_SERVER_SH no &

# give some time to startup
	sleep 15

# 2 warmup 
	echo "warmup for server"
#	for (( h=0; h<${apps_num}; h++ ))
#	do
#		app=${APPS[${h}]}
#		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
#		then
#			cd $CLIENT_DIR/nlp
#			$NLP_SH $app 1 1 1
#		else
#			cd $CLIENT_DIR/img
#			$IMG_SH $app 1 1 1
#		fi
#		sleep 1
#	done
# 3 the real experimnet
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}
	if [ $mon_flag -eq 1 ]
	then
	
		$MON_DIR/$MON_BIN $RESULT_DIR 0 $app-log.txt &
		mon_pid=$!
	fi
	sleep 1

	for iter in `seq 1 1`
	do

		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app 1 $batch 1 &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 $batch 1 &
		fi
		pids[${h}]=$!   

	for pid in ${pids[*]}; do 
		wait $pid   
	done # pid
	done # iter
	sleep 50

	if [ $mon_flag -eq 1 ]
	then
		kill -SIGINT $mon_pid
	fi

	done #app

# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/gpu.csv
	
fi 


####################
#3) third experiment : GPU Server (dedicated)
####################
if [ $flag_3 -eq 1 ]
then
#wait for previous server to fully shutdown
sleep 10


# start monitor if flag is setted
	if [ $mon_flag -eq 1 ]
	then
	for gpu_id in `seq 0 3`
	do
		$MON_DIR/$MON_BIN $RESULT_DIR $gpu_id $gpu_id-log.txt &
		mon_pid[${gpu_id}]=$!
	done
	sleep 1
	fi

# 1. start GPU server 
	cd $SERVER_DIR
	$GPU_SERVER_SH dedicated &

# give some time to startup
	sleep 15

# 2 warmup 
	echo "warmup for server"
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}

		if [ $app == 'ner' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app 1 1 1
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 1 1
		fi
		sleep 3
	done
# 3 the real experimnet
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}
		if [ $app == 'ner' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app $N 1 1 &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $N 1 1 &
		fi
		pids[${h}]=$!   

	done # app

	for pid in ${pids[*]}; do 
		wait $pid   
	done
	sleep 10
# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/dedicated_gpu.csv
	if [ $mon_flag -eq 1 ]
	then
	for pid in ${mon_pid[*]}; do
		kill -SIGINT $pid
	done
	sleep 1
	fi
fi
done #batch

