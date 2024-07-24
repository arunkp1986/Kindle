#!/bin/bash

cd ../

CWD=`pwd`
echo $CWD

GEMOS_DIR=$CWD"/gemOS"
GEM5_DIR=$CWD"/gem5"
OUTPUT_DIR=$CWD"/output"

if [ ! -f data_workloadb.img ]; then
    echo "data_workloadb.img not found, extract it from bench_diskimages"
    exit 0
fi

cd $GEMOS_DIR/include
cp dirty_1ms.h dirty.h
cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_ssp_sh_1ms.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/1ms/workloadb $GEMOS_DIR/gemOS_ssp_sh_1ms.kernel $CWD/data_workloadb.img > workloadb_ssp_1ms.out 2>&1
