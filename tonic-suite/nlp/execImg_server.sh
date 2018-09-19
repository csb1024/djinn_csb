#!/bin/bash

MON=/home/sbchoi/git/gpu-cloud/monitor/cpp3/nvml_mon

IMC=execImc.sh
FACE=execFace.sh
DIG=execDig.sh

SERVER_SCRIPT_ROOT=/home/sbchoi/org/djinn-1.0/service
IMG_ROOT=/home/sbchoi/org/djinn-1.0/tonic-suite/img
SERVER_SCRIPT=executeCPUServer.sh
LOG_DIR=$PWD/log_dir
#NVML_LOG_DIR=$PWD/nvml_log_dir
mkdir -p $LOG_DIR
rm $LOG_DIR/*
#mkdir -p $NVML_LOG_DIR
#rm $NVML_LOG_DIR/*


LARGE_BATCH=(1 2 4 8 16 32 64)
MED_BATCH=(1 2 4 8 16)
SMALL_BATCH=(1 2 4 8)
for cores in "${MED_BATCH[@]}"
do
DST_LOG_DIR=$PWD/$cores-core-logs
mkdir -p $DST_LOG_DIR
rm $DST_LOG_DIR/*

cd $SERVER_SCRIPT_ROOT
./$SERVER_SCRIPT $cores &
sleep 7
cd $IMG_ROOT
for i in "${SMALL_BATCH[@]}"
do
#$MON $NVML_LOG_DIR 0 &
#MON_PID=$!
#sleep 0.5
./execImc.sh 10 1 $i 2>$LOG_DIR/$i-imc-log
#kill -INT $MON_PID
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-imc-log.txt
done #i

for i in "${SMALL_BATCH[@]}"
do
#$MON $NVML_LOG_DIR 0 &
#MON_PID=$!
#sleep 0.5
./execFace.sh 10 1 $i 2>$LOG_DIR/$i-face-log
#kill -INT $MON_PID
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-face-log.txt
done #i

for i in "${LARGE_BATCH[@]}"
do
#$MON $NVML_LOG_DIR 0 &
#MON_PID=$!
#sleep 0.5
./execDig.sh 10 1 $i 2>$LOG_DIR/$i-dig-log
#kill -INT $MON_PID
#mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-dig-log.txt
done #i
mv $LOG_DIR/* $DST_LOG_DIR/ 
pkill djinn
sleep 7
done #cores
