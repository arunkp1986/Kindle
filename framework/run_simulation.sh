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

./run_Kindle.sh $CWD/output

