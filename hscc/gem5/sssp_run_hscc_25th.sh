#!/bin/bash

cd ../

CWD=`pwd`
echo $CWD

GEMOS_DIR=$CWD"/gemOS"
GEM5_DIR=$CWD"/gem5"
OUTPUT_DIR=$CWD"/output"

if [ ! -f data_sssp.img ]; then
    echo "data_sssp.img not found, extract it from bench_diskimages"
    exit 0
fi

cd $GEMOS_DIR
cp nonvolatile_25th.c nonvolatile.c
make clean
make
cp gemOS.kernel gemOS_hscc_25th.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/hscc/twentyfive/sssp $GEMOS_DIR/gemOS_hscc_25th.kernel $CWD/data_sssp.img > sssp_hscc_25th.out 2>&1
