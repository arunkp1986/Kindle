#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to copy run scripts"

cd run_scripts

cp *_case1*.sh ../gem5

cp *_case1*.c ../gem5

cd ../gem5

echo "Going to run Kindle simulation for Figure 4 a"


gcc run_pt_nvm_case1_128MB.c -o run_pt_nvm_case1_128MB

./run_pt_nvm_case1_128MB $CWD/output/pagetable/nvm/case1/128MB/gemos.out $CWD/output/pagetable/nvm/case1/128MB/gemos.err

gcc run_pt_nvm_case1_16MB.c -o run_pt_nvm_case1_16MB

./run_pt_nvm_case1_16MB $CWD/output/pagetable/nvm/case1/16MB/gemos.out $CWD/output/pagetable/nvm/case1/16MB/gemos.err

gcc run_pt_nvm_case1_32MB.c -o run_pt_nvm_case1_32MB

./run_pt_nvm_case1_32MB $CWD/output/pagetable/nvm/case1/32MB/gemos.out $CWD/output/pagetable/nvm/case1/32MB/gemos.err

gcc run_pt_nvm_case1_64MB.c -o run_pt_nvm_case1_64MB

./run_pt_nvm_case1_64MB $CWD/output/pagetable/nvm/case1/64MB/gemos.out $CWD/output/pagetable/nvm/case1/64MB/gemos.err

gcc run_pt_rebuild_case1_128MB.c -o run_pt_rebuild_case1_128MB

./run_pt_rebuild_case1_128MB $CWD/output/pagetable/rebuild/case1/128MB/gemos.out $CWD/output/pagetable/rebuild/case1/128MB/gemos.err

gcc run_pt_rebuild_case1_16MB.c -o run_pt_rebuild_case1_16MB

./run_pt_rebuild_case1_16MB $CWD/output/pagetable/rebuild/case1/16MB/gemos.out $CWD/output/pagetable/rebuild/case1/16MB/gemos.err

gcc run_pt_rebuild_case1_32MB.c -o run_pt_rebuild_case1_32MB

./run_pt_rebuild_case1_32MB $CWD/output/pagetable/rebuild/case1/32MB/gemos.out $CWD/output/pagetable/rebuild/case1/32MB/gemos.err

gcc run_pt_rebuild_case1_64MB.c -o run_pt_rebuild_case1_64MB

./run_pt_rebuild_case1_64MB $CWD/output/pagetable/rebuild/case1/64MB/gemos.out $CWD/output/pagetable/rebuild/case1/64MB/gemos.err


echo "Going to plot Figure 4 a"

cd $CWD/output/pagetable

python3 getcycles.py

gnuplot fig_pagetable_case1.p

mv figure_4_a.eps ./results
