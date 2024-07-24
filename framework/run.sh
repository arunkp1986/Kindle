#!/bin/bash

val=`cat /proc/sys/kernel/yama/ptrace_scope`

if [ "$val" != "0" ]; then
   echo "set /proc/sys/kernel/yama/ptrace_scope to 0 with sudo"
   exit;
fi

echo "############ performing preparation of Kindle ###############"

./run_preparation.sh

echo "########## performing simulation of Kindle #################"

./run_simulation.sh

echo "############ Kindle Done #########"
echo "############ check output folder #########"
