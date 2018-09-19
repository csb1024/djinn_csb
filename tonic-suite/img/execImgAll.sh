#!/bin/bash

MON=/home/sbchoi/git/gpu-cloud/monitor/cpp3/nvml_mon

IMC=execImc.sh
FACE=execFace.sh
DIG=execDig.sh

LOG_DIR=$PWD/log_dir
NVML_LOG_DIR=$PWD/nvml_log_dir
mkdir -p $LOG_DIR
rm $LOG_DIR/*
mkdir -p $NVML_LOG_DIR
rm $NVML_LOG_DIR/*

#for i in `seq 1 10`
#do
#$MON $NVML_LOG_DIR 0 &
#MON_PID=$!
#sleep 0.5
##./execImc.sh $i 0 1 &
#./execImc.sh $i 0 1 2>$LOG_DIR/$i-imc-log
#kill -INT $MON_PID
#wait 
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-imc-log.txt
#done
#
#for i in `seq 1 10`
#do
#$MON $NVML_LOG_DIR 0 &
#MON_PID=$!
#sleep 0.5
##./execDig.sh $i 0 1 &
##sleep 0.1
#./execDig.sh $i 0 1 2>$LOG_DIR/$i-dig-log
#kill -INT $MON_PID
#wait
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-dig-log.txt
#done
for i in `seq 1 10`
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
#./execFace.sh $i 0 1 &
#sleep 0.1
./execFace.sh $i 0 1 2>$LOG_DIR/$i-face-log 
kill -INT $MON_PID
wait
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-face-log.txt
done


