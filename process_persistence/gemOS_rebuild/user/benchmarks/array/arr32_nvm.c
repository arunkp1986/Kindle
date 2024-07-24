#include<ulib.h>

#define SIZE 4
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
    u64 input[SIZE] = {0};
    RDTSC_START();    
    for(int i=1; i<SIZE; i++){
        input[i] += input[i-1];
        asm volatile("sfence");
        asm volatile ("clwb (%0)":: "r"(&input[i]):);
    }
    RDTSC_STOP();
    u64 rdtsc_count = uelapsed(start_hi,start_lo,end_hi,end_lo);
    printf("rdtsc:%u\n",rdtsc_count);

    return 0;
}
