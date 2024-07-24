#!/bin/bash

CWD=`pwd`

echo "going to build gem5"

cd $CWD/gem5

scons build/X86/gem5.opt -j8

echo "going to build gemOS"

cd $CWD

cp init.c $CWD/gemOS/user/

cd $CWD/gemOS

make

cd $CWD

echo "running simulation"

gcc run_quick_sort.c -o run_quick_sort

./run_quick_sort $CWD/output/gemos.out $CWD/output/gemos.err

