#!/bin/bash
export OMP_NUM_THREADS=4

if [ -z $1 ]
then
echo "no scheduler was specified! default "no" will be used"
scheduler='no'
else
scheduler=$1
fi

if [ -z $2 ]
then
echo "no cpu execution mode was specified! default "PEB" will be used"
CMODE='PEB'
else
CMODE=$2
fi

if [ -z $3 ]
then
echo "no gpu execution mode  was specified! default "PEB" will be used"
GMODE='PEB'
else
GMODE=$3
fi


./djinn --common ../common/ --weights weights/ --portno 8080 --gpu 1 --debug 1 --nets nets.txt --ngpu 4 -s $scheduler --cpu_exec $CMODE --gpu_exec $GMODE
