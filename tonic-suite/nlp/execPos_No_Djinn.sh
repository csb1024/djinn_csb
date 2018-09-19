#!/bin/bash 

if [ -z "$1" ]
then
echo "Please specify how many times you want to call inference"
exit 1
fi 

if [ -z "$2" ]
then 
echo "Please specifiy waiting time between inferences"
exit
fi

if [ -z "$3" ]
then
echo "Please specify batch size"
exit
fi

N=$1
WAIT=$2
BATCH=$3

for i in `seq 1 $N`
do
#echo "Hi"

./tonic-nlp --task pos --network pos.prototxt --weight pos.caffemodel --input input/sanity-test-input.txt --djinn 0 --gpu 1 --batch $BATCH

sleep $WAIT
done
