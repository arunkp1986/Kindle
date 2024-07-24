/*
 *Here stream access to stack sets all bits in dirty bitmap
 *This test case is to check scalability of the dirty tracking scheme
 *
 * */
#include<ulib.h>

#define PAGE_SIZE 4096
#define SIZE (1UL<<12)
#define TARCK_SIZE 64

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{  
   char* ptr[10];
   int j = 0;
   while(j<25){
       char * addr = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
       munmap(addr,SIZE);
       for(int i=0; i<10; i++){
           ptr[i] = (char*)mmap(addr,SIZE,PROT_WRITE|PROT_READ,MAP_FIXED|MAP_NVM);
           if(ptr[i]<0){
               printf("mmap failed\n");
               exit(0);
	   }
           ptr[i][0] = i;
           addr = ptr[i]+(1UL<<30);
       }
       for(int i=0; i<10; i++){
           munmap(ptr[i],SIZE);
       }
       j++;
   }
   return 0;
}
