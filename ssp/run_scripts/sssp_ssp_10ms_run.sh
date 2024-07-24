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

cd $GEMOS_DIR/include
cp dirty_10ms.h dirty.h
cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_ssp_sh_10ms.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/10ms/sssp $GEMOS_DIR/gemOS_ssp_sh_10ms.kernel $CWD/data_sssp.img > sssp_ssp_10ms.out 2>&1
