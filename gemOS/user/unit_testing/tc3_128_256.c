/*
 *This benchmark accesses stack at distance of 256 bytes
 *Tracking granularity is 128 Bytes
 *The tracking bitmap will be 1010101010
 * */
#include<ulib.h>
#define PAGE_SIZE 4096
#define SIZE (8*PAGE_SIZE)
#define TRACK_SIZE 128

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   volatile char arr[SIZE];
   start_dtrack(TRACK_SIZE);
   for (int j=0; j<10; j++) {
      for (int i=0; i<SIZE; i+=256) {
         arr[i] = 'A';
      }
   }
   end_dtrack();
   sleep(1000);
   read_dbitmap(TRACK_SIZE);
   return 0;
}
