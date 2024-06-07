#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>


#define NUM_ELEMS (1000000)

unsigned lfsr_rand(unsigned seed){
    unsigned lfsr = seed;
    uint16_t bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

void quick_sort(uint32_t* a, int first, int last){
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

int main(){
    uint32_t num_elements = NUM_ELEMS;
    uint32_t seed = 0xACE1u;
    uint32_t rand_value = 0;
    uint32_t* input;
    input = (uint32_t*)mmap(NULL,num_elements*sizeof(uint32_t),PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    for(int i=0; i<num_elements; i++){
        rand_value = rand_value ? lfsr_rand(rand_value) : lfsr_rand(seed);
	input[i] = rand_value;
    }
    quick_sort(input,0,num_elements-1);
    for(int i=0; i<num_elements-1; i++){
        assert(input[i]<=input[i+1]);
    }
    return 0;
}
