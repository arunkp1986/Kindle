#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define LLC_SIZE (1<<21)
#define SIZE (512*1024)
#define NUM_ELEMS 256

int main()
{
    
    uint32_t num_elements = (SIZE>>2);
    uint32_t* input =(uint32_t*)mmap(NULL,SIZE,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0); 
    srand(0);
    int index = 0;
    for(int i=0; i<num_elements; i++){
	index = rand()%num_elements;
        input[index] += 1;
    }
    return 0;
}
