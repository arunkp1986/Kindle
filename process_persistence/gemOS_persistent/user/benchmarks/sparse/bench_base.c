#include<ulib.h>


#define NUM_ACCESS 100000
#define PAGE_SIZE 4096

unsigned lfsr_rand(unsigned seed){
    unsigned lfsr = seed;
    u16 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    u32 num_elements = 25000;
    register int count = 0;
    u32 seed = 0xACE1u;
    u32 rand_value = 0;
    u32 index = 0;
    u64 input[25000];
    for(int i=0; i<num_elements; i++){
        rand_value = rand_value ? lfsr_rand(rand_value) : lfsr_rand(seed);
	input[i] = rand_value%num_elements;
    }
    gem5_dump_stats();
    gem5_reset_stats();
    index = input[0];
    for(int i=0; i<(NUM_ACCESS/50); i++){
        count = 0;
        for(int j=0; j<50; j++){
            index = (input[index]*PAGE_SIZE)%(num_elements-1);
	    input[index] += 0;
        }
        while(count<1000){
	    count += 1;
	}
    }
    gem5_dump_stats();
    gem5_reset_stats();
    return 0;
}
