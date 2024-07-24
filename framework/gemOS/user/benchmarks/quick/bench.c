#include<ulib.h>

#define NUM_ACCESS 5000

unsigned lfsr_rand(unsigned seed){
    unsigned lfsr = seed;
    u16 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

void quick_sort(u32* a, int first, int last){
    int pivot = first;
    int i = first;
    int j = last;
    int temp;
    if(first < last){
        while(i<j){
	    while((a[i] <= a[pivot]) && (i < last))
                i++;
	    while((a[j] > a[pivot]) && (j > first))
                j--;
	    if(i < j){
                temp = a[i];
		a[i] = a[j];
		a[j] = temp;
	    }
	} 
        temp = a[j];
        a[j] = a[pivot];
        a[pivot] = temp;
        quick_sort(a, first, j-1);
        quick_sort(a, j+1, last);
    }
}
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5){
    u32 num_elements = 10000;
    u32 seed = 0xACE1u;
    u32 rand_value = 0;
    register int count = 0;
    u32* input = (u32*)mmap(NULL,num_elements*sizeof(u32),PROT_READ|PROT_WRITE,0);
    if(!input){
        printf("mmap error\n");
	exit(0);
    }
    for(int i=0; i<num_elements; i++){
        rand_value = rand_value ? lfsr_rand(rand_value) : lfsr_rand(seed);
	input[i] = rand_value;
    }
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_start();
    for(int i=0; i<NUM_ACCESS; i++){
	count = 0;
        quick_sort(input,0,num_elements-1);
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
