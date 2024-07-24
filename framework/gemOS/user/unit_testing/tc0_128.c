/*
 *Here stream access to stack sets all bits in dirty bitmap
 *This test case is to check scalability of the dirty tracking scheme
 *
 * */
#include<ulib.h>

#define PAGE_SIZE 4096
#define SIZE (8*PAGE_SIZE)
#define TARCK_SIZE 128

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   volatile char arr[SIZE];
   start_dtrack(TARCK_SIZE);
   
   for (int j=0; j<10; j++) {
      for (int i=0; i<SIZE; i+=1) {
         arr[i] = 'A';
      }
   }
   
   end_dtrack();
   sleep(1000);
   read_dbitmap(TARCK_SIZE);

   return 0;
}
