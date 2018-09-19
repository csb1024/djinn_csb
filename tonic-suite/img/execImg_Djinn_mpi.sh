#!/bin/bash 

if [ -z "$1" ]
then
echo "Please specify the image application you want to call"
exit 1
fi 

if [ -z "$2" ]
then
echo "Please specify how many times each client process does inference "
exit 1
fi 


if [ -z "$3" ]
then
echo "Please specify how many client process you want to make"
exit 1
fi 
if [ -z "$4" ]
then 
echo "Please specifiy batch size"
exit
fi



APP=$1
REQUESTS=$2
N=$3
BATCH=$4




#echo "Hi"
mpiexec --bind-to-none -n $N ./tonic-img-mpi --task $APP --network "$APP".prototxt --weight "$APP".caffemodel --input "$APP"-list2.txt --djinn 1 --gpu 1  --portno 8080 --batch $BATCH -r $REQUESTS
