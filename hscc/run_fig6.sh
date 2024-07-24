#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to copy run scripts"

cd run_scripts

cp gapbs_run_hscc*.sh ../gem5

cp run_gapbs_hscc*.c ../gem5

cp sssp_run_hscc_*.sh ../gem5

cp run_sssp_hscc_*.c ../gem5

cp workloadb_run_hscc_*.sh ../gem5

cp run_workloadb_hscc_*.c ../gem5

cd ../gem5

echo "Going to run Kindle simulation for Figure 6"


#gcc run_gapbs_hscc_5th.c -o run_gapbs_hscc_5th

#./run_gapbs_hscc_5th $CWD/output/hscc/five/gapbs/gemos.out $CWD/output/hscc/five/gapbs/gemos.err

#gcc run_gapbs_hscc_25th.c -o run_gapbs_hscc_25th

#./run_gapbs_hscc_25th $CWD/output/hscc/twentyfive/gapbs/gemos.out $CWD/output/hscc/twentyfive/gapbs/gemos.err

#gcc run_gapbs_hscc_50th.c -o run_gapbs_hscc_50th

#./run_gapbs_hscc_50th $CWD/output/hscc/fifty/gapbs/gemos.out $CWD/output/hscc/fifty/gapbs/gemos.err

gcc run_gapbs_hscc_nomig.c -o run_gapbs_hscc_nomig

./run_gapbs_hscc_nomig $CWD/output/hscc/nomigration/gapbs/gemos.out $CWD/output/hscc/nomigration/gapbs/gemos.err

gcc run_sssp_hscc_5th.c -o run_sssp_hscc_5th

./run_sssp_hscc_5th $CWD/output/hscc/five/sssp/gemos.out $CWD/output/hscc/five/sssp/gemos.err

gcc run_sssp_hscc_25th.c -o run_sssp_hscc_25th

./run_sssp_hscc_25th $CWD/output/hscc/twentyfive/sssp/gemos.out $CWD/output/hscc/twentyfive/sssp/gemos.err

gcc run_sssp_hscc_50th.c -o run_sssp_hscc_50th

./run_sssp_hscc_50th $CWD/output/hscc/fifty/sssp/gemos.out $CWD/output/hscc/fifty/sssp/gemos.err

gcc run_sssp_hscc_nomig.c -o run_sssp_hscc_nomig

./run_sssp_hscc_nomig $CWD/output/hscc/nomigration/sssp/gemos.out $CWD/output/hscc/nomigration/sssp/gemos.err

gcc run_workloadb_hscc_5th.c -o run_workloadb_hscc_5th

./run_workloadb_hscc_5th $CWD/output/hscc/five/workloadb/gemos.out $CWD/output/hscc/five/workloadb/gemos.err

gcc run_workloadb_hscc_25th.c -o run_workloadb_hscc_25th

./run_workloadb_hscc_25th $CWD/output/hscc/twentyfive/workloadb/gemos.out $CWD/output/hscc/twentyfive/workloadb/gemos.err

gcc run_workloadb_hscc_50th.c -o run_workloadb_hscc_50th

./run_workloadb_hscc_50th $CWD/output/hscc/fifty/workloadb/gemos.out $CWD/output/hscc/fifty/workloadb/gemos.err

gcc run_workloadb_hscc_nomig.c -o run_workloadb_hscc_nomig

./run_workloadb_hscc_nomig $CWD/output/hscc/nomigration/workloadb/gemos.out $CWD/output/hscc/nomigration/workloadb/gemos.err


echo "Going to plot Figure 6"

cd $CWD/output

python3 getcycles_hscc_simSec.py

gnuplot fig_hscc_threshold.p

mv hscc_threshold.eps ./results
