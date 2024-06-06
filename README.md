# Hybrid memory framework

## Create disk image and template code

Execute **run.sh** to generate disk image with memory access trace of sample program **quick\_sort**

Change *BENCH\_PATH* , *BENCH\_NAME* , *BENCH\_ARGS* variables in **run.sh** to create disk image for another program. These variables are used in *execlp* call in **benchmark\_run.c**
