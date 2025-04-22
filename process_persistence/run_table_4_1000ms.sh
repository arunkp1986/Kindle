#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to copy run scripts"

cd run_scripts

#cp *_case5*.sh ../gem5

#cp *_case5*.c ../gem5

cd ../gem5

echo "Going to run Kindle simulation for Table-3"


#gcc run_pt_nvm_case5_eight_512MB_1000ms.c -o run_pt_nvm_case5_eight_512MB_1000ms

#./run_pt_nvm_case5_eight_512MB_1000ms $CWD/output/pagetable/nvm/case5/eight_512MB_1000ms/gemos.out $CWD/output/pagetable/nvm/case5/eight_512MB_1000ms/gemos.err


#gcc run_pt_nvm_case5_four_512MB_1000ms.c -o run_pt_nvm_case5_four_512MB_1000ms

#./run_pt_nvm_case5_four_512MB_1000ms $CWD/output/pagetable/nvm/case5/four_512MB_1000ms/gemos.out $CWD/output/pagetable/nvm/case5/four_512MB_1000ms/gemos.err

#gcc run_pt_nvm_case5_two_512MB_1000ms.c -o run_pt_nvm_case5_two_512MB_1000ms

#./run_pt_nvm_case5_two_512MB_1000ms $CWD/output/pagetable/nvm/case5/two_512MB_1000ms/gemos.out $CWD/output/pagetable/nvm/case5/two_512MB_1000ms/gemos.err

#gcc run_pt_rebuild_case5_eight_512MB_1000ms.c -o run_pt_rebuild_case5_eight_512MB_1000ms

#./run_pt_rebuild_case5_eight_512MB_1000ms $CWD/output/pagetable/rebuild/case5/eight_512MB_1000ms/gemos.out $CWD/output/pagetable/rebuild/case5/eight_512MB_1000ms/gemos.err

#gcc run_pt_rebuild_case5_four_512MB_1000ms.c -o run_pt_rebuild_case5_four_512MB_1000ms

#./run_pt_rebuild_case5_four_512MB_1000ms $CWD/output/pagetable/rebuild/case5/four_512MB_1000ms/gemos.out $CWD/output/pagetable/rebuild/case5/four_512MB_1000ms/gemos.err

gcc run_pt_rebuild_case5_two_512MB_1000ms.c -o run_pt_rebuild_case5_two_512MB_1000ms

./run_pt_rebuild_case5_two_512MB_1000ms $CWD/output/pagetable/rebuild/case5/two_512MB_1000ms/gemos.out $CWD/output/pagetable/rebuild/case5/two_512MB_1000ms/gemos.err

#echo "Going to generate Table 3"

#cd $CWD/output/pagetable

#python3 getcycles_case5.py

#mv table_3.tsv ./results
