#!/bin/bash

MON=/home/sbchoi/git/gpu-cloud/monitor/cpp3/nvml_mon
LOG_DIR=$PWD/log_dir
ORG_LOG_DIR=$PWD/egs/voxforge/s5/exp//djinn_decode/decode_local/log
mkdir -p $LOG_DIR
rm $LOG_DIR/*
NVML_LOG_DIR=$PWD/nvml_log_dir
mkdir -p $NVML_LOG_DIR
rm $NVML_LOG_DIR/*

batches=(1 2 4 8)
epoches=(8 4 2 1)
for i in `seq 0 3`
do
i=$((3-i))
batch=${batches[$i]}
epoch=${epoches[$i]}


$MON $NVML_LOG_DIR 0 &
MON_PID=$!
sleep 0.5
ts=$(date +%s%N) 
for j in `seq  1 ${epoch}`
do

./tonic-asr $batch
 sleep 0.5
done #j
tt=$((($(date +%s%N) - $ts)/1000000))
echo "Time taken: $tt milliseconds"
cat $ORG_LOG_DIR/decode.1.log >> $LOG_DIR/$batch-asr-log
kill -INT $MON_PID
mv $NVML_LOG_DIR/monitor_log.txt $NVML_LOG_DIR/$i-asr-log.txt
done #i

