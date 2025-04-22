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

cd $GEMOS_DIR
cp nonvolatile_50th.c nonvolatile.c
make clean
make
cp gemOS.kernel gemOS_hscc_50th.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1


./run_nobypass.sh $OUTPUT_DIR/hscc/fifty/workloadb $GEMOS_DIR/gemOS_hscc_50th.kernel $CWD/data_workloadb.img > workloadb_hscc_50th.out 2>&1
