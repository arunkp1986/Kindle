#!/bin/bash

cd ../

CWD=`pwd`
echo $CWD

GEMOS_DIR=$CWD"/gemOS_persistent"
GEM5_DIR=$CWD"/gem5"
OUTPUT_DIR=$CWD"/output"

if [ ! -f gemos.img ]; then
    echo "gemos.img not found, extract it from bench_diskimages"
    exit 0
fi

cd $GEMOS_DIR/user
cp init_nvm_case2_4KB.c init.c
cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_nvm_case2_4KB.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/pagetable/nvm/case2/4KB $GEMOS_DIR/gemOS_nvm_case2_4KB.kernel $CWD/gemos.img > nvm_case2_4KB.out 2>&1

