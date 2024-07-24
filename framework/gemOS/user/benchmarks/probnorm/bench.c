#include<ulib.h>

#define NUM_ACCESS 5000

unsigned lfsr_rand(unsigned seed){
    unsigned lfsr = seed;
    u16 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    u32 num_elements = 10000;
    register int count = 0;
    u32 seed = 0xACE1u;
    u32 rand_value = 0;
    u32 index = 0;
    u32 input[10000];
    int access_nums = 100;
    int access_array[100] = {62,19,44,70,80,62,78,102,57,41,82,80,53,27,67,80,72,69,91,70,33,32,54,60,55,94,49,48,92,98,54,15,54,83,69,96,73,49,54,46,60,94,54,48,51,68,79,63,73,54,97,57,63,53,66,47,53,66,89,49,78,65,88,18,79,62,76,27,89,64,73,36,61,28,77,59,44,53,55,94,50,69,52,54,32,79,60,79,78,62,90,57,124,93,77,82,71,42,82,24}; 
    for(int i=0; i<num_elements; i++){
        rand_value = rand_value ? lfsr_rand(rand_value) : lfsr_rand(seed);
	input[i] = rand_value%num_elements;
    }
    
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_start();
    for(int k=0; k<NUM_ACCESS; k++){
        for(int i=0; i<access_nums; i++){
            index = input[0];
	    count = 0;
            for(int j=0; j<access_array[i]; j++){
                index = input[index];
                input[index] += 0;
	    }
	    while(count<1000){
                count += 1;
	    }
	}
    }
    checkpoint_end();
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_stats();
    return 0;
}
