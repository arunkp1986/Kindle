/*
 *Here stream access to stack sets all bits in dirty bitmap
 *This test case is to check scalability of the dirty tracking scheme
 *
 * */
#include<ulib.h>

#define PAGE_SIZE 4096
#define SIZE (1UL<<28)
#define TARCK_SIZE 64

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{  
   checkpoint_start();
   char * ptr = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   for(int i=0; i<SIZE; i+=PAGE_SIZE){
       ptr[i] = i%10;
   }
   int ret = mprotect(ptr,SIZE>>3,PROT_READ);
   if(ret<0){
       printf("mprotect failed\n");
       exit(-1);
   }
   int sum = 0;
   for(int i=0; i<SIZE; i+=PAGE_SIZE){
       sum += ptr[i];
   }
   munmap(ptr,SIZE);
   checkpoint_end();
   //checkpoint_stats();
   return 0;
}
