# Kindle: A Comprehensive Framework for Exploring OS-Architecture Interplay in Hybrid Memory Systems

Kindle, based on gem5 and gemOS, to explore and prototype research ideas in hybrid memory systems crossing the hardware-software boundaries, and perform comprehensive empirical evaluation. Kindle provides mechanisms to realize process persistence in hybrid memory systems while facilitating analysis of different design challenges and alternatives.

## Create disk image and template code
=======================================

Execute **run.sh** to generate disk image with memory access trace of sample benchmark program **quick\_sort**

`./run.sh`

Change *BENCH\_PATH* , *BENCH\_NAME* , *BENCH\_ARGS* variables in **run.sh** to create disk image for another benchmark program. These variables are used in *execlp* call in **benchmark\_run.c**

**run.sh** executes **benchmark\_run** to generate trace of provided benchmark program (quick\_sort). The tracefile is cauptured in **pinatrace.out** file. The file size of ~22 GB for **quick\_sort**


**run.sh** executes **format\_file.py** python program to format records in **pinatrace.out** to create **data.csv**. Size of **data.csv** is ~12GB for **quick\_sort**

Python script also creates template code for gemOS, **init.c**

**run.sh** then creates a *data.img* file equal to the size of **data.csv**. Then **readfile** loads **data.csv** to *data.img*, which is used as disk image for gem5.

## Build gem5 using instructions 
=====================================

1> The **gem5** folder contains **hybrid memory** modifications, build gem5 by following below link, except the step to clone gem5 source "git clone https://gem5.googlesource.com/public/gem5"

https://www.gem5.org/documentation/learning_gem5/part1/building/

## Build gemOS 
==============================

1> run **make** inside gemOS

2> user space code is writte in **user/init.c**

3> Use the template **init.c** generated in previous step for user space.

## Run Kindle
======================================

1> use **data.img** file as disk image.

2> use **run_Kindle.sh** script as below to start **Kindle**,**run_Kindle.sh** takes **output** folder as argument to save gem5 stats, create an ouput folder and pass complete path as follows.

**run_Kindle.sh path_to_output**

3> Now **gem5** starts and you need to connect to gem5 using telnet in a different terminal

**telnet localhost 3456**, the port number is printed by gem5 as " Listening for connections on port 3456"

4> The terminal after connecting to "telnet" will show gemOS prompt as below

**GemOS#**

5> To run your user program in **init.c**, type **init** command on **GemOS#** as below

**GemOS# init**

6> The init program executes and you can see output

7> To exit **gemOS** run, **exit** command as below on **GemOS#**

**GemOS# exit**

## Run SSP, HSCC prototypes
=======================================

1> SSP **gem5** and **gemOS** changes are present in **ssp** folder

2> The SSP userspace benchmark **init\_micro-benchmark.c** is present in **ssp/gemOS/user/** folder

3> Disk images for benchmarks applications are present in **bench_diskimages**

4> unzip the disk image and pass it as argument to **--disk-image** argument in **run_Kindle.sh**

5> change **gem5.opt** to point to **gem5** inside ssp folder in **run_Kindle.sh**

6> Follow steps under **Run Kindle** to start Kindle

7> HSCC **gem5** and **gemOS** changes are present in **hscc** folder, follow same steps as **ssp** to run **hscc**

**Note**: We have provided object files of **gemOS**, please contact authors if you want ".c" files to change **gemOS**

[Debadatta Mishra](https://www.cse.iitk.ac.in/users/deba/)

[Arun KP](https://www.cse.iitk.ac.in/users/kparun/)

