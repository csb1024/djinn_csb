#!/bin/bash

MON=/home/sbchoi/git/gpu-cloud/monitor/cpp3/nvml_mon
LOG_DIR=$PWD/log_dir
NVML_LOG_DIR=$PWD/nvml_log_dir
mkdir -p $LOG_DIR
rm $LOG_DIR/*
mkdir -p $NVML_LOG_DIR   
rm $NVML_LOG_DIR/*

#for i in `seq 1 20`
#do
#$MON $NVML_LOG_DIR 0 &   
#MON_PID=$!
#sleep 0.5 
#./execNer.sh $i 0 1 2>$LOG_DIR/$i-ner-log
#kill -INT $MON_PID 
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-ner-log.txt
#done
#
#for i in `seq 1 20`
#do
#$MON $NVML_LOG_DIR 0 & 
#MON_PID=$!
#sleep 0.5 
#./execPos.sh $i 0 1 2>$LOG_DIR/$i-pos-log
#kill -INT $MON_PID  
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-pos-log.txt
#done
#
for i in `seq 1 20`
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
 ./execChk.sh $i 0 1 2>$LOG_DIR/$i-chk-log
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-chk-log.txt
done
