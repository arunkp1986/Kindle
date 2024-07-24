#!/bin/bash

CWD=`pwd`

$CWD/gem5/build/X86/gem5.opt --outdir=$CWD/output $CWD/gem5/configs/example/fs.py --mem-size=3GB --nvm-size=2GB --caches --l3cache --cpu-type TimingSimpleCPU --hybrid-channel True --mem-type=DDR4_2400_16x4 --nvm-type=NVM_2666_1x64 --kernel=$CWD/gemOS/gemOS.kernel --disk-image=$CWD/data.img > quick_sort.out 2>&1

