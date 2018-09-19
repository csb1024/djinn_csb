#!/bin/bash

MON=/home/sbchoi/git/gpu-cloud/monitor/cpp3/nvml_mon
LOG_DIR=$PWD/log_dir
ORG_LOG_DIR=$PWD/egs/voxforge/s5/exp//djinn_decode/decode_local/log
mkdir -p $LOG_DIR
rm $LOG_DIR/*
NVML_LOG_DIR=$PWD/nvml_log_dir
mkdir -p $NVML_LOG_DIR
rm $NVML_LOG_DIR/*
for i in `seq 1 8`
do

$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
for j in `seq  1 11`
do
 ./tonic-asr $i
 sleep 0.5
cat $ORG_LOG_DIR/decode.1.log >> $LOG_DIR/$i-asr-log
done #j
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-asr-log.txt



done #i

