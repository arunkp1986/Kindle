#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to copy run scripts"

cd run_scripts

cp *_case4*.sh ../gem5

cp *_case4*.c ../gem5

cd ../gem5

echo "Going to run Kindle simulation for Figure 4 b"


gcc run_pt_nvm_case4_eight_512MB.c -o run_pt_nvm_case4_eight_512MB

./run_pt_nvm_case4_eight_512MB $CWD/output/pagetable/nvm/case4/eight_512MB/gemos.out $CWD/output/pagetable/nvm/case4/eight_512MB/gemos.err

gcc run_pt_nvm_case4_four_512MB.c -o run_pt_nvm_case4_four_512MB

./run_pt_nvm_case4_four_512MB $CWD/output/pagetable/nvm/case4/four_512MB/gemos.out $CWD/output/pagetable/nvm/case4/four_512MB/gemos.err

gcc run_pt_nvm_case4_two_512MB.c -o run_pt_nvm_case4_two_512MB

./run_pt_nvm_case4_two_512MB $CWD/output/pagetable/nvm/case4/two_512MB/gemos.out $CWD/output/pagetable/nvm/case4/two_512MB/gemos.err

gcc run_pt_rebuild_case4_eight_512MB.c -o run_pt_rebuild_case4_eight_512MB

./run_pt_rebuild_case4_eight_512MB $CWD/output/pagetable/rebuild/case4/eight_512MB/gemos.out $CWD/output/pagetable/rebuild/case4/eight_512MB/gemos.err

gcc run_pt_rebuild_case4_four_512MB.c -o run_pt_rebuild_case4_four_512MB

./run_pt_rebuild_case4_four_512MB $CWD/output/pagetable/rebuild/case4/four_512MB/gemos.out $CWD/output/pagetable/rebuild/case4/four_512MB/gemos.err

gcc run_pt_rebuild_case4_two_512MB.c -o run_pt_rebuild_case4_two_512MB

./run_pt_rebuild_case4_two_512MB $CWD/output/pagetable/rebuild/case4/two_512MB/gemos.out $CWD/output/pagetable/rebuild/case4/two_512MB/gemos.err

echo "Going to generate Table 3"

cd $CWD/output/pagetable

python3 getcycles_case4.py

mv table_3.tsv ./results
