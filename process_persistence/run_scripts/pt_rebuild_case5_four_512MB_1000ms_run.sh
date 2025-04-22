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

cd $GEMOS_DIR/include
cp dirty_1000ms.h dirty.h 

cd $GEMOS_DIR/user
cp init_rebuild_case5_four_512MB.c init.c
cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_rebuild_case5_four_512MB_1000ms.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/pagetable/rebuild/case5/four_512MB_1000ms $GEMOS_DIR/gemOS_rebuild_case5_four_512MB_1000ms.kernel $CWD/gemos.img > rebuild_case5_four_512MB_1000ms.out 2>&1

