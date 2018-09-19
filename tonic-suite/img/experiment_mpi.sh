APPS=('imc' 'dig' 'face')
#APPS=('face' 'imc')
#APPS=('imc' 'dig')
#APPS=('dig')
#NUMS=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)
NUMS=(2 4 8 16 32)
GLOBAL_LOG_DIR=$PWD/log_dir
for i in `seq 1 3`
do
LOG_DIR=$PWD/log_dir_"$i"
mkdir -p $LOG_DIR
rm $LOG_DIR/*
for app in "${APPS[@]}"
do
for num in "${NUMS[@]}"
do
echo testing $num "$app"s on GPU
#./execImg_No_Djinn_mpi.sh $app $num 1 2>$LOG_DIR/$app-$num-log
./execImg_Djinn_mpi.sh $app $num 1 2>$LOG_DIR/$app-$num-log
cp $LOG_DIR/$app-$num-log $GLOBAL_LOG_DIR/$app-$num-log
sleep 3
done # num
sleep 3
done # app
sleep 3
done #i 
