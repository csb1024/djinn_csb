#!/bin/bash

log_dir=$PWD/logs
mkdir -p $log_dir

server_dir=/home/sbchoi/org/djinn-1.0/service

bubble_dir=/home/sbchoi/git/gpu-bubble
bubble_bin=gbubble

intensitys=(0 10 20 30 40 50 60 70 80 90 100)
intensitys_num=${#intensitys[@]}

#image benchmarks related variables
image_benchmarks=('imc' 'face' 'dig')
img_bench_num=${#image_benchmarks[@]}
img_dir=$PWD/img

for batch in `seq 1 8`
do
for (( h=0; h<${img_bench_num}; h++ ));
do
	img_bench=${image_benchmarks[${h}]}

echo "profiling "$img_bench
for (( j=0; j<${intensitys_num}; j++ ));
do
intensity=${intensitys[${j}]}
echo "bubble intensity: "$intensity

if [ $intensity -ne 0 ]
then
	$bubble_dir/$bubble_bin $intensity &
	sleep 3
fi

for i in `seq 1 20`
do
#	echo "do somgething"
	cd $img_dir
	./execImg_Djinn_no_mpi.sh $img_bench 1 $batch
	sleep 1
done #repeat n times

if [ $intensity -ne 0 ]
then
	wait
fi

done #intensity
done #img benchmarks 
cp $server_dir/log.txt $log_dir/${batch}.csv
done #batch size
