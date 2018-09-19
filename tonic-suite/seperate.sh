#!/bin/bash

ORG_DIR=$PWD/results
DEST_DIR=$PWD/monitor_results

cp -R $ORG_DIR $DEST_DIR

# erase all files that end with .txt in org_dir
for subdir in `ls $ORG_DIR`
do
cd $ORG_DIR/$subdir
rm *.txt
done
#erase all files that end with .csv in dest_dir
for subdir in `ls $DEST_DIR`
do
cd $DEST_DIR/$subdir
rm *.csv
done



