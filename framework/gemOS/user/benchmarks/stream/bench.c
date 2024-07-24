#include<ulib.h>

#define LLC_SIZE (1<<21)
#define NUM_ACCESS 10000000


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
    u32 seed = 0xACE1u;
    u32 rand_value = 0;
    u32 index = 0;
    register int count = 0;
    u32 input[25000];
        
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_start();

    for(int i=0; i<(NUM_ACCESS/100); i++){
	count = 0;
	for(int j=0; j<100; j++){
	    index = (i*100+j)%(num_elements-1);
	    input[index] += 1;
	}
	while(count<1000){
	    count += 1;
	}
    }

    checkpoint_end();
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_stats();
    return 0;
}
