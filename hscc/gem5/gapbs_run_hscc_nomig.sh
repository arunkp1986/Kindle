#!/bin/bash

cd ../

CWD=`pwd`
echo $CWD

GEMOS_DIR=$CWD"/gemOS"
GEM5_DIR=$CWD"/gem5"
OUTPUT_DIR=$CWD"/output"

if [ ! -f data_gapbs.img ]; then
    echo "data_gapbs.img not found, extract it from bench_diskimages"
    exit 0
fi

cd $GEMOS_DIR
cp nonvolatile_No.c nonvolatile.c
make clean
make
cp gemOS.kernel gemOS_hscc_nomig.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/hscc/nomigration/gapbs $GEMOS_DIR/gemOS_hscc_nomig.kernel $CWD/data_gapbs.img > gapbs_hscc_nomig.out 2>&1

