#!/bin/bash

echo "copying disk image"

cp ../bench_diskimages/gemos.img.xz .

xz -d gemos.img.xz


echo "building gem5"

cd gem5

git checkout kindle

scons build/X86/gem5.opt -j 8
