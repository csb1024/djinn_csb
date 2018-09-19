#!/bin/bash 

if [ -z "$1" ]
then
echo "Please specify the NLP application you want to call"
exit 1
fi 

if [ -z "$2" ]
then
echo "Please specify how many times you want to call inference"
exit 1
fi 

if [ -z "$3" ]
then 
echo "Please specifiy batch size"
exit
fi
if [ -z "$4" ]
then 
echo "Please average delay between request(in seconds)"
exit
fi

APP=$1
N=$2
BATCH=$3
MEAN=$4



#echo "Hi"
./tonic-nlp --task $APP --network "$APP".prototxt --weight "$APP".caffemodel --input input/sanity-test-input.txt --djinn 1 --gpu 1  --portno 8080 --batch $BATCH -r $N -m $MEAN
