#include<ulib.h>

#define LLC_SIZE (1<<21)
#define NUM_ACCESS 1000000

unsigned lfsr_rand(unsigned seed){
    unsigned lfsr = seed;
    u16 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    u32 num_elements = 25600;
    register int count = 0;
    u32 seed = 0xACE1u;
    u32 rand_value = 0;
    u32 index = 0;
    u32 input[25600];
    for(int i=0; i<num_elements; i++){
        rand_value = rand_value ? lfsr_rand(rand_value) : lfsr_rand(seed);
	input[i] = rand_value%num_elements;
    }
    checkpoint_start();
    gem5_dump_stats();
    gem5_reset_stats();
    index = input[0];
    for( int i=0; i<(NUM_ACCESS/50); i++){
        for(int j=0; j<50; j++){
            index = input[index];
	    input[index] += 0;
	}
    }
    checkpoint_end();
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_stats();
    return 0;
}
