/*
 *Here stream access to stack sets all bits in dirty bitmap
 *This test case is to check scalability of the dirty tracking scheme
 *
 * */
#include<ulib.h>

#define PAGE_SIZE 4096
#define SIZE (1UL<<30)
#define TARCK_SIZE 64

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{  
   char * ptr = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   for(int i=0; i<SIZE; i+=PAGE_SIZE){
       ptr[i] = i%10;
   }

   munmap(ptr,SIZE>>1);
   char * ptr1 = (char*)mmap(NULL,SIZE>>1,PROT_WRITE|PROT_READ,MAP_NVM);
   
   for(int i=0; i<(SIZE>>1); i+=PAGE_SIZE){
       ptr1[i] = i%10;
   }
   
   munmap((ptr+(SIZE>>1)),SIZE>>1);

   char * ptr2 = (char*)mmap(NULL,SIZE>>1,PROT_WRITE|PROT_READ,MAP_NVM);
   
   for(int i=0; i<(SIZE>>1); i+=PAGE_SIZE){
       ptr2[i] = i%10;
   }

   int sum = 0;
   for(int i=0; i<(SIZE>>1); i+=PAGE_SIZE){
       sum += ptr1[i];
   }
   munmap(ptr1,SIZE>>1);

   for(int i=0; i<(SIZE>>1); i+=PAGE_SIZE){
       sum += ptr2[i];
   }
   munmap(ptr2,SIZE>>1);
   //printf("sum:%d\n",sum);
   return 0;
}
