#!/bin/bash


CWD=`pwd`

echo $CWD

echo "Going to generate Table 4"

cd $CWD/output

python3 page_migrated_hscc.py

mv table4.tsv ./results
