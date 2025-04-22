/*
 *Here stream access to stack sets all bits in dirty bitmap
 *This test case is to check scalability of the dirty tracking scheme
 *
 * */
#include<ulib.h>

#define PAGE_SIZE 4096
#define SIZE (1UL<<12)
#define TARCK_SIZE 64
#define ITERATION 10000

#define RDTSC_START()            \
        __asm__ volatile("RDTSCP\n\t" \
                         "mov %%edx, %0\n\t" \
                         "mov %%eax, %1\n\t" \
                         : "=r" (start_hi), "=r" (start_lo) \
                         :: "%rax", "%rbx", "%rcx", "%rdx");

#define RDTSC_STOP()              \
        __asm__ volatile("RDTSCP\n\t" \
                         "mov %%edx, %0\n\t" \
                         "mov %%eax, %1\n\t" \
                         "CPUID\n\t" \
                         : "=r" (end_hi), "=r" (end_lo) \
                         :: "%rax", "%rbx", "%rcx", "%rdx");

u64 uelapsed(u32 start_hi, u32 start_lo, u32 end_hi, u32 end_lo)
{
        u64 start = (((u64)start_hi) << 32) | start_lo;
        u64 end   = (((u64)end_hi)   << 32) | end_lo;
        return end-start;
}


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   u32 start_hi = 0, start_lo = 0, end_hi = 0, end_lo = 0;
   u64 exectime = 0;
   checkpoint_start();
   char * ptr = (char*)mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_NVM);
   int j = 0;
   while(j<ITERATION){
       RDTSC_START();
       *ptr = 10;
       RDTSC_STOP();
       exectime += uelapsed(start_hi,start_lo,end_hi,end_lo);
       if(!flush_pte(ptr)){
           printf("PTE flush failed\n");
       }
       j += 1;
   }
   munmap(ptr,SIZE);
   checkpoint_end();
   printf("execution time:%lu\n",exectime);
   //checkpoint_stats();
   return 0;
}
