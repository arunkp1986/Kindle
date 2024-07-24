#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
   if(argc<3){
       printf("pass gemos.out gemos.err path\n");
       exit(0);
   }
   printf("output:%s\n",argv[1]);
   printf("error:%s\n",argv[2]);
   char cmd[100];
   int wstatus;
   int pid = fork();
   if (!pid){
       strcpy(cmd,"./gapbs_run_hscc_50th.sh");
       system(cmd);
   }
   else{
       sleep(5);
       int pid2 = fork();
       if (!pid2){
           printf("child\n");
           int out_fd = open(argv[1],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
           int err_fd = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
           if (dup2(out_fd,STDOUT_FILENO)<0 || dup2(err_fd,STDERR_FILENO)<0){
              printf("dup failed\n");
              exit(0);
           }
           strcpy(cmd,"port=$(cat gapbs_hscc_50th.out|grep 'connections on port'|cut -f7 -d ' '); ./run.except $port");
           system(cmd);
       }
       else{
           waitpid(pid,&wstatus,WUNTRACED|WCONTINUED);
       }
   }
   return 0;
}
