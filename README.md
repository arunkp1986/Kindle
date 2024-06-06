# Hybrid memory framework

## Create disk image and template code

Execute **run.sh** to generate disk image with memory access trace of sample benchmark program **quick\_sort**

Change *BENCH\_PATH* , *BENCH\_NAME* , *BENCH\_ARGS* variables in **run.sh** to create disk image for another program. These variables are used in *execlp* call in **benchmark\_run.c**

**run.sh** executes **benchmark\_run** to generate trace of provided benchmark program (quick\_sort). The tracefile is cauptured in **pinatrace.out** file. The file size of ~22 GB.


**run.sh** executes **format\_file.py** python program to format records in **pinatrace.out** to create **data.csv**, and also template code for gemOS.

**run.sh** then creates a *data.img* file and excutes **readfile** to load **data.csv** to *data.img*, disk image for gem5
