#include<ulib.h>

#define REC_COUNT 100

struct data{
        unsigned interval;
        unsigned offset;
        char ops;
        int size;
        char type[6];
	int index;
    };

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    char* H33 = (char *)mmap(NULL,8437760, PROT_READ|PROT_WRITE, 0);
    u64 H_Addr[34];
    H_Addr[33] = (u64)H33;
    char S0[135168];
    u64 S_Addr[2];
    S_Addr[0] = (u64)S0;
    //char* S0 = &SS0;
    unsigned size = sizeof(struct data)*REC_COUNT;
    unsigned num_blocks = (size%4096)?(size>>12)+1:(size>>12);
    char* input = (char *)mmap(NULL, num_blocks*4096, PROT_READ|PROT_WRITE,MAP_POPULATE);
    if(input < 0){
        printf("mmap failed\n");
	exit(0);
    }
    for(int blk_off = 0; blk_off < num_blocks; blk_off++){
        read_blk((char*)((u64)input+(blk_off<<12)), blk_off);
    }
    struct data *dt = (struct data*)input;
    int j = 0;
    int iter = 0;
    while(j<REC_COUNT){
        printf("interval:%u, offset:%u, ops:%c, size:%u, type:%s, index:%d\n",dt->interval,dt->offset,dt->ops,dt->size,dt->type,dt->index);
	if(dt->ops == 'R'){
	    iter = 0;
	    if(dt->type[0] == 'S'){
	        unsigned long addr = S_Addr[dt->index]+dt->offset;
	        while(iter < dt->size){
		    printf("stack addr:%lx\n",addr+iter);
	            char read = *(char*)(addr+iter);
		    iter += 1;
		}
	    }
	    if(dt->type[0] == 'H'){
	        unsigned long addr = H_Addr[dt->index]+dt->offset;
	        while(iter < dt->size){
		    printf("heap addr:%lx\n",addr+iter);
	            char read = *(char*)(addr+iter);
		    iter += 1;
		}
	    }
	}else if(dt->ops == 'W'){
	    iter = 0;
	    if(dt->type[0] == 'S'){
	        unsigned long addr = S_Addr[dt->index]+dt->offset;
	        while(iter < dt->size){
		    printf("stack addr:%lx\n",addr+iter);
	            *(char*)(addr+iter) = 1;
		    iter += 1;
		}
	    }
	    if(dt->type[0] == 'H'){
	        unsigned long addr = H_Addr[dt->index]+dt->offset;
	        while(iter < dt->size){
		    printf("heap addr:%lx\n",addr+iter);
	            *(char*)(addr+iter) = 1;
		    iter += 1;
		}
	    }
	}
	j += 1;
	dt++;
    }
    return 0;
}
