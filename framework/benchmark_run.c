#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "benchmark_run.h"

int main(int argc, char * argv[]){
    if(argc<3){
        printf("pass PIN path and PIN tool path\n");
	exit(0);
    }
    const char* bench_path = getenv("BENCH_PATH");
    const char* bench_name = getenv("BENCH_NAME");
    const char* bench_args = getenv("BENCH_ARGS");
    if(!bench_path || !bench_name){
        printf("set BENCH_PATH and BENCH_NAME environment variables\n");
	exit(0);
    }
    int wstatus;
    int pid;
    pid = fork();
    if(pid){
        printf("pid:%u\n",pid);
        int pid2;
        pid2 = fork();
        if(!pid2){
	    char pid_str[10];
            sprintf(pid_str,"%u",pid);
            if(execlp(argv[1],"pin","-pid",pid_str,"-t",argv[2],(char*)NULL) == -1){
                perror("exec failed: "); 
	        exit(-1);
	    }
	}
	sleep(5);	
	char filename[50];
	char data[8192];
	char read_data[81920];
	sprintf(filename,"/proc/%d/maps",pid);
	int maps_fd = open("maps",O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXO);
	if(maps_fd < 0){
	    perror("open failed: ");
	    exit(-1);
	}
        int iter = 0;	
	int read_size = 0;
	int write_size = 0;
	int offset = 0;
        //printf("wait is called by %u\n:",getpid());
        //waitpid(pid,&wstatus,0);
	do{
	    memset(&data,0,8192);
	    int proc_fd = open(filename,O_RDONLY);
            if(proc_fd < 0){
                perror("open failed: ");
                goto WRITE;
	    }
	    memset(&read_data,0,81920);
	    offset = 0;
            while(read_size=read(proc_fd,data,8192)){ 
	         //printf("%s\n",data);
		 memcpy(read_data+offset,data,read_size);
		 offset += read_size;
		/*if(write(maps_fd,data,read_size) < 0){
	            perror("maps file write failed: ");
	            exit(-1);
		 }*/
	    }
	    /*
	    if((read_size=read(proc_fd,data,81920)) < 0){
	        perror("file read failed: ");
	        exit(-1);
	    }
	    printf("********");
	    if(write(maps_fd,data,read_size) < 0){
	        perror("maps file write failed: ");
	        exit(-1);
	    }*/
            sleep(20);
	    iter += 1;
	    close(proc_fd);
        }while(iter<10);
WRITE:
       if((write_size=write(maps_fd,read_data,offset)) < 0){
	        perror("maps file write failed: ");
	        exit(-1);
	}
	printf("mmap file write size: %u\n",write_size);
	close(maps_fd);
        waitpid(pid,&wstatus,0);
    }
    else{
	if(execlp(bench_path,bench_name,bench_args,(char*)NULL) == -1){
            perror("exec failed: ");
	    exit(-1);
	}
    }
   return 0;
}
