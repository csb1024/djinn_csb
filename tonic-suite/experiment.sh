
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
#Experiment related variables
#MEANS=( '1' '0.1' '0.01' '0.00125')
MEANS=(  '0.01')
means_num=${#MEANS[@]}




APPS=('dig' 'imc' 'face' 'ner' 'chk' 'pos')

#APPS=('dig' 'imc' 'face')
#APPS=('ner' 'chk' 'pos')
#APPS=('face')
apps_num=${#APPS[@]}

#FLAGS
mon_flag=0 # flag for running monitor
server_flag=0 # flag for starting up server
flag_1=0 # GPU_PEB , all_gpu
flag_2=0 # CON_PEB, fixed
flag_3=0 # PEB_PEB, fixed
flag_4=1 # EB_EB, fixed

# # of requests for this experiment

NREQ=5000

for ((i=0; i<${means_num}; i++))
do
mean=${MEANS[${i}]}
batch=1
#for mean in $MEANS
#do
ROOT_RESULT_DIR=$CLIENT_DIR/results/$mean

#####################
#1)experiment : GPU_PEB, all_GPU
#####################
if [ $flag_1 -eq 1 ]
then
RESULT_DIR=$ROOT_RESULT_DIR/ALL_GPU
mkdir -p $RESULT_DIR

#wait for previous server to fully shutdown
sleep 5

# start monitor if flag is setted
	

if [ $server_flag -eq 1 ]
then
# 1. start GPU s1rver 
	cd $SERVER_DIR
	$GPU_SERVER_SH all_gpu PEB PEB &

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
			$NLP_SH $app 1 1 0
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 1 0
		fi
		sleep 1
	done
fi
# 3 the real experimnet
	if [ $mon_flag -eq 1 ]
	then
	mon_num=4
 	for (( k=0; k<${mon_num}; k++ ))
	do
		$MON_DIR/$MON_BIN $RESULT_DIR 0 $k-log.txt &
		mon_pids[${k}]=$!
	
	done
	sleep 2
	fi
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}


		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app $NREQ $batch $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $NREQ $batch $mean &
		fi
		pids[${h}]=$!   
	done 
	for pid in ${pids[*]}; do 
		wait $pid   
	done # pid
	sleep 60
	if [ $mon_flag -eq 1 ]
	then
	for mon_pid in ${mon_pids[*]}; do 
		kill -SIGINT $mon_pid
	done #mon_pid
	fi


# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/gpu.csv
	
fi 


#####################
#2) experiment : CON_PEB, fixed
#####################
if [ $flag_2 -eq 1 ]
then
RESULT_DIR=$ROOT_RESULT_DIR/CON_PEB
mkdir -p $RESULT_DIR


#wait for previous server to fully shutdown
sleep 5

# start monitor if flag is setted
	

if [ $server_flag -eq 1 ]
then
# 1. start GPU s1rver 
	cd $SERVER_DIR
	$GPU_SERVER_SH fixed CON PEB &

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
			$NLP_SH $app 1 1 0
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 1 0
		fi
		sleep 1
	done
fi
# 3 the real experimnet
	if [ $mon_flag -eq 1 ]
	then
	mon_num=4
 	for (( k=0; k<${mon_num}; k++ ))
	do
		$MON_DIR/$MON_BIN $RESULT_DIR 0 $k-log.txt &
		mon_pids[${k}]=$!
	
	done
	sleep 2
	fi
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}


		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app $NREQ $batch $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $NREQ $batch $mean &
		fi
		pids[${h}]=$!   
	done 
	for pid in ${pids[*]}; do 
		wait $pid   
	done # pid
	sleep 60
	if [ $mon_flag -eq 1 ]
	then
	for mon_pid in ${mon_pids[*]}; do 
		kill -SIGINT $mon_pid
	done #mon_pid
	fi


# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/gpu.csv
	
fi 

#####################
#3) experiment : PEB_PEB , fixed
#####################
if [ $flag_3 -eq 1 ]
then
RESULT_DIR=$ROOT_RESULT_DIR/PEB_PEB
mkdir -p $RESULT_DIR


#wait for previous server to fully shutdown
sleep 5

# start monitor if flag is setted
	

if [ $server_flag -eq 1 ]
then
# 1. start GPU s1rver 
	cd $SERVER_DIR
	$GPU_SERVER_SH fixed PEB PEB &

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
			$NLP_SH $app 1 1 0
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 1 0
		fi
		sleep 1
	done
fi
# 3 the real experimnet
	if [ $mon_flag -eq 1 ]
	then
	mon_num=4
 	for (( k=0; k<${mon_num}; k++ ))
	do
		$MON_DIR/$MON_BIN $RESULT_DIR 0 $k-log.txt &
		mon_pids[${k}]=$!
	
	done
	sleep 2
	fi
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}


		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app $NREQ $batch $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $NREQ $batch $mean &
		fi
		pids[${h}]=$!   
	done 
	for pid in ${pids[*]}; do 
		wait $pid   
	done # pid
	sleep 60
	if [ $mon_flag -eq 1 ]
	then
	for mon_pid in ${mon_pids[*]}; do 
		kill -SIGINT $mon_pid
	done #mon_pid
	fi


# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/gpu.csv
	
fi 
#####################
#4) experiment : EB_EB, fixed
#####################
if [ $flag_4 -eq 1 ]
then
RESULT_DIR=$ROOT_RESULT_DIR/EB_EB
mkdir -p $RESULT_DIR
#wait for previous server to fully shutdown
sleep 5

# start monitor if flag is setted
	

if [ $server_flag -eq 1 ]
then
# 1. start GPU s1rver 
	cd $SERVER_DIR
	$GPU_SERVER_SH fixed EB EB &

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
			$NLP_SH $app 1 1 0
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app 1 1 0
		fi
		sleep 1
	done
fi
# 3 the real experimnet
	if [ $mon_flag -eq 1 ]
	then
	mon_num=4
 	for (( k=0; k<${mon_num}; k++ ))
	do
		$MON_DIR/$MON_BIN $RESULT_DIR 0 $k-log.txt &
		mon_pids[${k}]=$!
	
	done
	sleep 2
	fi
	for (( h=0; h<${apps_num}; h++ ))
	do
		app=${APPS[${h}]}


		if [ $app == 'ner' -o $app == 'pos' -o $app == 'chk' ]
		then
			cd $CLIENT_DIR/nlp
			$NLP_SH $app $NREQ $batch $mean &
		else
			cd $CLIENT_DIR/img
			$IMG_SH $app $NREQ $batch $mean &
		fi
		pids[${h}]=$!   
	done 
	for pid in ${pids[*]}; do 
		wait $pid   
	done # pid
	sleep 60
	if [ $mon_flag -eq 1 ]
	then
	for mon_pid in ${mon_pids[*]}; do 
		kill -SIGINT $mon_pid
	done #mon_pid
	fi


# 4 stop server and clean up data 
	pkill djinn
	mv $SERVER_DIR/log.txt $RESULT_DIR/gpu.csv
	
fi 


done #means

