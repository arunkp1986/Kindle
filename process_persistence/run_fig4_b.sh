#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to copy run scripts"

cd run_scripts

cp *_case2*.sh ../gem5

cp *_case2*.c ../gem5

cd ../gem5

echo "Going to run Kindle simulation for Figure 4 b"


gcc run_pt_nvm_case2_1GB.c -o run_pt_nvm_case2_1GB

./run_pt_nvm_case2_1GB $CWD/output/pagetable/nvm/case2/1GB/gemos.out $CWD/output/pagetable/nvm/case2/1GB/gemos.err

gcc run_pt_nvm_case2_2MB.c -o run_pt_nvm_case2_2MB

./run_pt_nvm_case2_2MB $CWD/output/pagetable/nvm/case2/2MB/gemos.out $CWD/output/pagetable/nvm/case2/2MB/gemos.err

gcc run_pt_nvm_case2_4KB.c -o run_pt_nvm_case2_4KB

./run_pt_nvm_case2_4KB $CWD/output/pagetable/nvm/case2/4KB/gemos.out $CWD/output/pagetable/nvm/case2/4KB/gemos.err

gcc run_pt_rebuild_case2_1GB.c -o run_pt_rebuild_case2_1GB

./run_pt_rebuild_case2_1GB $CWD/output/pagetable/rebuild/case2/1GB/gemos.out $CWD/output/pagetable/rebuild/case2/1GB/gemos.err

gcc run_pt_rebuild_case2_2MB.c -o run_pt_rebuild_case2_2MB

./run_pt_rebuild_case2_2MB $CWD/output/pagetable/rebuild/case2/2MB/gemos.out $CWD/output/pagetable/rebuild/case2/2MB/gemos.err

gcc run_pt_rebuild_case2_4KB.c -o run_pt_rebuild_case2_4KB

./run_pt_rebuild_case2_4KB $CWD/output/pagetable/rebuild/case2/4KB/gemos.out $CWD/output/pagetable/rebuild/case2/4KB/gemos.err


echo "Going to plot Figure 4 b"

cd $CWD/output/pagetable

python3 getcycles_case2.py

gnuplot fig_pagetable_case2.p

mv figure_4_b.eps ./results
