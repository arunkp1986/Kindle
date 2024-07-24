#!/bin/bash

CWD=`pwd`

echo $CWD

echo "Going to build pin"

cd $CWD/pin-3.20-98437-gf02b61307-gcc-linux/source/tools/MyPinTool
make

cd $CWD

echo "Going to compile benchmark_run"

make clean
make

echo "Going to compile quick sort benchmark"

gcc quick_sort.c -o quick_sort

export BENCH_PATH=$CWD"/quick_sort"
export BENCH_NAME="quick_sort"
export BENCH_ARGS=" "

./benchmark_run $CWD/pin-3.20-98437-gf02b61307-gcc-linux/pin $CWD/pin-3.20-98437-gf02b61307-gcc-linux/source/tools/MyPinTool/obj-intel64/vanillareadwrite.so


if [ ! -f pinatrace.out ]; then
    echo "Pin Trace File not found!";
    exit 1
fi

echo "Going to format trace file"

export ELEMS=$(python format_file.py pinatrace.out)

echo "number of records in trace: $ELEMS"

#export ELEMS=482697203

re='^[0-9]+$'
if ! [[ $ELEMS =~ $re ]] ; then
   echo "error: Not a number" >&2; exit 1
fi

SIZE=$(stat --printf=%s data.csv)

echo "data.csv file size $SIZE"

MBSIZE=$(expr $SIZE / 1048576 + 1)

echo "going to create data image file of size  $MBSIZE*1M"

dd if=/dev/zero of=data.img bs=1M count=$MBSIZE

g++ readfile.cpp -o readfile

./readfile
