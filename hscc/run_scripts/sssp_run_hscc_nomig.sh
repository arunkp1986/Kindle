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
cp nonvolatile_No.c nonvolatile.c
make clean
make
cp gemOS.kernel gemOS_hscc_nomig.kernel


./run_nobypass.sh $OUTPUT_DIR/hscc/nomigration/sssp $GEMOS_DIR/gemOS_hscc_nomig.kernel $CWD/data_sssp.img > sssp_hscc_nomig.out 2>&1
