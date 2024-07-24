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
cp init_nvm_case4_two_512MB.c init.c
cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_nvm_case4_two_512MB.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/pagetable/nvm/case4/two_512MB $GEMOS_DIR/gemOS_nvm_case4_two_512MB.kernel $CWD/gemos.img > nvm_case4_two_512MB.out 2>&1

