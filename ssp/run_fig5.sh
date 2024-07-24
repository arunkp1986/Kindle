#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to copy run scripts"

cd run_scripts

cp gapbs_ssp_*.sh ../gem5
cp run_gapbs_ssp_*.c ../gem5
cp gapbs_vanilla_run.sh ../gem5_vanilla
cp run_gapbs_vanilla.c ../gem5_vanilla

cp sssp_ssp_*.sh ../gem5
cp run_sssp_ssp*.c ../gem5
cp sssp_vanilla_run.sh ../gem5_vanilla
cp run_sssp_vanilla.c ../gem5_vanilla

cp workloadb_ssp_*.sh ../gem5
cp run_workloadb_ssp*.c ../gem5
cp workloadb_vanilla_run.sh ../gem5_vanilla
cp run_workloadb_vanilla.c ../gem5_vanilla

cd ../gem5

echo "Going to run Kindle simulation for Figure 6"


gcc run_gapbs_ssp_10ms.c -o run_gapbs_ssp_10ms

./run_gapbs_ssp_10ms $CWD/output/10ms/gapbs/gemos.out $CWD/output/10ms/gapbs/gemos.err

gcc run_gapbs_ssp_5ms.c -o run_gapbs_ssp_5ms

./run_gapbs_ssp_5ms $CWD/output/5ms/gapbs/gemos.out $CWD/output/5ms/gapbs/gemos.err

gcc run_gapbs_ssp_1ms.c -o run_gapbs_ssp_1ms

./run_gapbs_ssp_1ms $CWD/output/1ms/gapbs/gemos.out $CWD/output/1ms/gapbs/gemos.err

gcc run_sssp_ssp_10ms.c -o run_sssp_ssp_10ms

./run_sssp_ssp_10ms $CWD/output/10ms/sssp/gemos.out $CWD/output/10ms/sssp/gemos.err

gcc run_sssp_ssp_5ms.c -o run_sssp_ssp_5ms

./run_sssp_ssp_5ms $CWD/output/5ms/sssp/gemos.out $CWD/output/5ms/sssp/gemos.err

gcc run_sssp_ssp_1ms.c -o run_sssp_ssp_1ms

./run_sssp_ssp_1ms $CWD/output/1ms/sssp/gemos.out $CWD/output/1ms/sssp/gemos.err

gcc run_workloadb_ssp_10ms.c -o run_workloadb_ssp_10ms

./run_workloadb_ssp_10ms $CWD/output/10ms/workloadb/gemos.out $CWD/output/10ms/workloadb/gemos.err

gcc run_workloadb_ssp_5ms.c -o run_workloadb_ssp_5ms

./run_workloadb_ssp_5ms $CWD/output/5ms/workloadb/gemos.out $CWD/output/5ms/workloadb/gemos.err

gcc run_workloadb_ssp_1ms.c -o run_workloadb_ssp_1ms

./run_workloadb_ssp_1ms $CWD/output/1ms/workloadb/gemos.out $CWD/output/1ms/workloadb/gemos.err

cd ../gem5_vanilla

gcc run_gapbs_vanilla.c -o run_gapbs_vanilla

./run_gapbs_vanilla $CWD/output/vanilla/gapbs/gemos.out $CWD/output/vanilla/gapbs/gemos.err

gcc run_sssp_vanilla.c -o run_sssp_vanilla

./run_sssp_vanilla $CWD/output/vanilla/sssp/gemos.out $CWD/output/vanilla/sssp/gemos.err

gcc run_workloadb_vanilla.c -o run_workloadb_vanilla

./run_workloadb_vanilla $CWD/output/vanilla/workloadb/gemos.out $CWD/output/vanilla/workloadb/gemos.err

echo "Going to plot Figure 6"

cd $CWD/output

python3 getcycles_ssp.py

gnuplot fig_ssp_interval.p

mv ssp_interval.eps ./results
