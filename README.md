# Hybrid memory framework

## Create disk image and template code
=======================================

Execute **run.sh** to generate disk image with memory access trace of sample benchmark program **quick\_sort**

`./run.sh`

Change *BENCH\_PATH* , *BENCH\_NAME* , *BENCH\_ARGS* variables in **run.sh** to create disk image for another benchmark program. These variables are used in *execlp* call in **benchmark\_run.c**

**run.sh** executes **benchmark\_run** to generate trace of provided benchmark program (quick\_sort). The tracefile is cauptured in **pinatrace.out** file. The file size of ~22 GB for **quick\_sort**


**run.sh** executes **format\_file.py** python program to format records in **pinatrace.out** to create **data.csv**. Size of **data.csv** is ~12GB for **quick\_sort**

Python script also creates template code for gemOS, **init.c**

**run.sh** then creates a *data.img* file equal to the size of **data.csv**. Then **readfile** loads **data.csv** to *data.img*, which is used as disk image for gem5.
