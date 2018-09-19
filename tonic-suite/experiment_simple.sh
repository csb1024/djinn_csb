
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
MEANS=('1' '0.5' '0.25' '0.1')
#MEANS=( '1')
means_num=${#MEANS[@]}
APPS=('dig' 'imc' 'face' 'ner')
apps_num=${#APPS[@]}

#FLAGS
mon_flag=1 # flag for running monitor
flag_1=1
flag_2=1
flag_3=1

# N : number of requests per client
N=32

for iter in `seq 1 10`
do
for ((j=0; j<${means_num}; j++))
do
mean=${MEANS[${j}]}
RESULT_DIR=$CLIENT_DIR/results/$iter-$mean
mkdir -p $RESULT_DIR
####################
#1) first experiment : CPU Server Only 
####################
if [ $flag_1 -eq 1 ]
then
# 1 start server with 4 open blas threads 
	sleep 10
	cd $SERVER_DIR
	$GPU_SERVER_SH cpu_only &

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

	# run processes and store pids in array

	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}

		if [ $app == 'ner' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app $N 1 $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $N 1 $mean &
		fi
		pids[${h}]=$!
	done # app
	# wait for all pids
	for pid in ${pids[*]}; do
		wait $pid
	done
	sleep 10

# 4 stop server and clean up data 
	pkill djinn 
	mv $SERVER_DIR/log.txt $RESULT_DIR/cpu.csv
	# start monitor if flag is setted


fi


#####################
#2) second experiment : GPU Server (shared)
#####################
if [ $flag_2 -eq 1 ]
then
#wait for previous server to fully shutdown
sleep 10

# start monitor if flag is setted
	if [ $mon_flag -eq 1 ]
	then
	
		$MON_DIR/$MON_BIN $RESULT_DIR 0 shared-log.txt &
		mon_pid=$!
	fi
	sleep 1



# 1. start GPU server 
	cd $SERVER_DIR
	$GPU_SERVER_SH no &

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
			$NLP_SH $app $N 1 $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $N 1 $mean &
		fi
		pids[${h}]=$!   

	done # app

	for pid in ${pids[*]}; do 
		wait $pid   
	done
	sleep 10
# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/shared_gpu.csv
	if [ $mon_flag -eq 1 ]
	then
		kill -SIGINT $mon_pid
	fi

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
			$NLP_SH $app $N 1 $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $N 1 $mean &
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
done # mean
done #iter

