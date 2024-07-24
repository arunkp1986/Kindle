#!/bin/bash

cd ../

CWD=`pwd`
echo $CWD

GEMOS_DIR=$CWD"/gemOS_vanilla"
GEM5_DIR=$CWD"/gem5_vanilla"
OUTPUT_DIR=$CWD"/output"

if [ ! -f data_sssp.img ]; then
    echo "data_sssp.img not found, extract it from bench_diskimages"
    exit 0
fi

cd $GEMOS_DIR
make clean
make
cp gemOS.kernel gemOS_vanilla.kernel

cd $GEM5_DIR
rm PhyMem.0
rm PhyMem.1

./run_nobypass.sh $OUTPUT_DIR/vanilla/sssp $GEMOS_DIR/gemOS_vanilla.kernel $CWD/data_sssp.img > sssp_va.out 2>&1
