#!/bin/bash
rm $PWD/med_dir/*
rm $PWD/max_dir/*

for  i in `seq 1 5`
do
log_dir=log_dir_"$i"
out_file=out_med_"$i".csv 
python parseLogs.py $log_dir $out_file med
mv $out_file $PWD/med_dir
out_file=out_max_"$i".csv 
python parseLogs.py $log_dir $out_file max
mv $out_file $PWD/max_dir 
done

python analyzeCSV.py $PWD/max_dir max.csv
echo "max results stored in max.csv"
python analyzeCSV.py $PWD/med_dir med.csv
echo "median results stored in med.csv"


