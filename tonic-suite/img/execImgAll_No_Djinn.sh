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

LARGE_BATCH=(1 2 4 8 16 32 64)
SMALL_BATCH=(1 2 4 8)

for i in "${SMALL_BATCH[@]}"
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
./execImc_No_Djinn.sh 10 1 $i 2>$LOG_DIR/$i-imc-log
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-imc-log.txt
done

for i in "${SMALL_BATCH[@]}"
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
./execFace_No_Djinn.sh 10 1 $i 2>$LOG_DIR/$i-face-log 
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-face-log.txt
done


for i in "${LARGE_BATCH[@]}"
do
$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
./execDig_No_Djinn.sh 10 1 $i 2>$LOG_DIR/$i-dig-log
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-dig-log.txt
done
