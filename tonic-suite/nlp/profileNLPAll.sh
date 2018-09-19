#!/bin/bash

MON=/home/sbchoi/git/gpu-cloud/monitor/cpp3/nvml_mon

#IMC=execImc.sh
#FACE=execFace.sh
#DIG=execDig.sh

#SERVER_SCRIPT_ROOT=/home/sbchoi/org/djinn-1.0/service
#NLP_ROOT=/home/sbchoi/org/djinn-1.0/tonic-suite/nlp
#SERVER_SCRIPT=executeCPUServer.sh
LOG_DIR=$PWD/log_dir
NVML_LOG_DIR=$PWD/nvml_log_dir
mkdir -p $LOG_DIR
rm $LOG_DIR/*
mkdir -p $NVML_LOG_DIR
rm $NVML_LOG_DIR/*


#LARGE_BATCH=(1 2 4 8 16 32 64 128 256)

for i in `seq 1 64`
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
./execNer.sh 20 1 $i 2>$LOG_DIR/$i-ner-log
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-ner-log.txt
done #i
#
for i in `seq 1 64`
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
./execPos.sh 20 1 $i 2>$LOG_DIR/$i-pos-log
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-pos-log.txt
done #i

#for i in `seq 1 64`
#do
#$MON $NVML_LOG_DIR 0 &
#MON_PID=$!
#sleep 0.5
#./execChk.sh 10 1 $i 2>$LOG_DIR/$i-chk-log
#kill -INT $MON_PID
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-chk-log.txt
#done #i
