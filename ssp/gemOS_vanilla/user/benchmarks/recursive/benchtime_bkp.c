#include<ulib.h>

#define LLC_SIZE (1<<21)
#define DEPTH 8
#define NUM_ACCESS 10

//how many recursive calls happened in each checkpoint?
//
void func(u16* elem, int index, int pos){
    u16 size = *(elem+index);
    char data[4096];
    register int count = 0;
    while(count < 1000){
        count += 1;
    }
    for(int i=0; i<size; i++){
        data[i] = '0';
    }
        if(pos){
        pos = pos-1;
	index = index+1;
        func(elem, index, pos);
    }
    count = 0;
    while(count < 1000){
        count += 1;
    }
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    u16 elements[512] = {72,80,30,46,36,33,68,73,67,105,66,80,52,102,19,74,56,55,85,114,63,75,88,39,67,52,88,73,64,88,53,42,74,76,55,66,46,53,53,49,73,110,63,70,101,43,42,3,34,28,66,69,58,52,73,71,70,57,85,48,56,77,88,40,86,90,76,62,86,76,50,79,31,15,30,57,93,109,108,88,46,36,74,32,48,55,50,11,13,58,58,81,45,79,57,21,77,89,40,90,72,56,17,76,81,57,31,94,49,76,71,68,82,67,61,56,79,95,61,67,81,88,29,81,55,66,84,53,64,93,78,33,111,47,77,73,44,87,59,43,43,21,87,93,19,83,87,79,74,53,48,66,91,38,10,34,40,85,83,87,74,103,58,56,42,63,46,38,64,25,76,53,69,72,69,57,65,105,44,68,64,71,113,65,101,45,56,74,58,71,12,43,48,51,36,39,90,42,113,18,68,52,68,80,8,45,49,58,79,52,90,54,68,19,91,18,65,85,54,54,56,42,70,66,104,63,74,93,69,66,83,76,51,63,87,135,56,55,59,60,75,80,47,70,72,45,98,73,72,38,121,72,60,38,55,26,86,39,50,49,42,71,62,105,60,30,95,56,95,78,67,32,72,55,67,59,82,53,41,52,49,14,43,31,61,76,77,99,72,42,19,55,37,67,64,77,62,85,73,77,57,60,81,61,127,32,76,91,83,56,59,47,60,54,81,66,65,73,57,41,42,87,68,62,43,26,105,54,62,57,121,53,70,49,38,71,51,76,63,78,48,39,53,66,69,36,55,53,82,72,38,64,84,79,102,103,52,75,65,89,95,70,61,53,7,84,53,115,55,58,65,44,64,117,54,98,76,60,71,39,43,71,89,65,44,46,76,72,71,35,75,76,67,93,88,103,69,61,59,94,73,57,56,51,73,92,21,65,69,100,46,81,87,90,67,62,64,63,129,88,53,44,73,71,59,61,45,65,76,58,76,100,86,96,87,54,58,68,54,59,85,60,68,60,42,31,92,42,71,67,65,55,84,50,59,57,39,15,70,72,79,64,104,40,51,58,68,51,73,47,68,79,46,33,69,66,79,94,88,75,71,28,83,105,36,11,20,52,59,57,39,66,41,66,60,50,65,67,63,70,87,111,24,84,82,74,85,38,65,46,83,32};
            
    checkpoint_start();
    gem5_dump_stats();
    gem5_reset_stats();
    for(int i=0; i<NUM_ACCESS; i++){
        for(int j=0; j<512; j+=DEPTH){
            func(elements, j, DEPTH);
	}
    }
    checkpoint_end();
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_stats();
    return 0;
}
