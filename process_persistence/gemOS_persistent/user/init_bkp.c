#include<ulib.h>

#define LLC_SIZE (1<<21)
#define NUM_ACCESS 10


unsigned lfsr_rand(unsigned seed){
    unsigned lfsr = seed;
    u16 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

struct data{
        unsigned interval;
        unsigned offset;
        char ops;
        int size;
        int type;
    };

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    unsigned size = sizeof(struct data)*1000010;
    char* input = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE,MAP_POPULATE);
    input[0] = 'A';
    read_blk(input, 0);
    struct data *dt = (struct data*)input;
    printf("Interval: %u, offset:%u, ops:%c, size:%d, type:%d\n", dt->interval, dt->offset, dt->ops, dt->size, dt->type);

    printf("size:%u\n",size);
	//long addr = expand(1, MAP_WR);
    /*
    u32 num_elements = 25600;
    u32 seed = 0xACE1u;
    u32 rand_value = 0;
    u32 index = 0;
    register int count = 0;
    u32 input[25600];

    checkpoint_start();
    gem5_dump_stats();
    gem5_reset_stats();
    for(int i=0; i<NUM_ACCESS; i++){
	for(int j=0; j<num_elements; j++){
	    input[j] += 1;
	}
    }
    checkpoint_end();
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_stats();*/
    return 0;
}
