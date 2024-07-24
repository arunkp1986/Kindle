/*
 *Here access at page granularity, so the dirtybitmap will be
 *have only 1 bit set for a page.
 *here the dirty bitmap will be ...000100...0001000...
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
      for (int i=0; i<SIZE; i+=PAGE_SIZE) {
         arr[i+2048] = 'A';
      }
   }
   end_dtrack();
   sleep(1000);
   read_dbitmap(TRACK_SIZE);
   return 0;
}
