/*
 *Here access is at track size granularity, so all bits in
 *dirty bitmap should be set. This case dirty bitmap will
 *be equivalent to access all bytes in array.
 * */

#include<ulib.h>
#define PAGE_SIZE 4096
#define SIZE (8*PAGE_SIZE)
#define TRACK_SIZE 512

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   volatile char arr[SIZE];
   start_dtrack(TRACK_SIZE);
   
   for (int j=0; j<10; j++) {
      for (int i=0; i<SIZE; i+=TRACK_SIZE) {
         arr[i] = 'A';
      }
   }
   
   end_dtrack();
   sleep(1000);
   read_dbitmap(TRACK_SIZE);
   return 0;
}
