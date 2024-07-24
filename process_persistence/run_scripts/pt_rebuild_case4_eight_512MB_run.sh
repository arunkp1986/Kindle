#!/bin/bash

cd ../

CWD=`pwd`
GEMOS_DIR=$CWD"/gemOS_rebuild"
GEM5_DIR=$CWD"/gem5"
OUTPUT_DIR=$CWD"/output"

if [ ! -f gemos.img ]; then
    echo "gemos.img not found, extract it from bench_diskimages"
    exit 0
fi

cd $GEMOS_DIR/user
cp init_rebuild_case4_eight_512MB.c init.c
cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_rebuild_case4_eight_512MB.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/pagetable/rebuild/case4/eight_512MB $GEMOS_DIR/gemOS_rebuild_case4_eight_512MB.kernel $CWD/gemos.img > rebuild_case4_eight_512MB.out 2>&1

