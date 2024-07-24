/*
 *Here stream access to stack sets all bits in dirty bitmap
 *This test case is to check scalability of the dirty tracking scheme
 *
 * */
#include<ulib.h>

#define PAGE_SIZE 4096
#define SIZE (1UL<<27)
#define TARCK_SIZE 64

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{  
   char* ptr[8];
   ptr[0] = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   if(ptr[0]<0){
       printf("mmap failed\n");
       exit(0);
   }
   for(int i=0; i < SIZE; i+=PAGE_SIZE){
       ptr[0][i] = i;
   }
   ptr[1] = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   if(ptr[1]<0){
       printf("mmap failed\n");
       exit(0);
   }
   for(int i=0; i < SIZE; i+=PAGE_SIZE){
       ptr[1][i] = i;
   }
   ptr[2] = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   if(ptr[2]<0){
       printf("mmap failed\n");
       exit(0);
   }
   for(int i=0; i < SIZE; i+=PAGE_SIZE){
       ptr[2][i] = i;
   }
   
   ptr[3] = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   if(ptr[3]<0){
       printf("mmap failed\n");
       exit(0);
   }
   for(int i=0; i < SIZE; i+=PAGE_SIZE){
       ptr[3][i] = i;
   }
   
   return 0;
}
