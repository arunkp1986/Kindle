#!/bin/bash

echo "copying disk files"

cp ../bench_diskimages/data_* .

xz -d data_gapbs.img.xz

xz -d data_sssp.img.xz

xz -d data_workloadb.img.xz

echo "build gem5"

cd gem5

scons build/X86/gem5.opt -j8
