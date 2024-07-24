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
u64 H_Addr[44];
u64 S_Addr[1];
char* H0 = (char *)mmap(NULL,2449408, PROT_READ|PROT_WRITE, 0);
H_Addr[0] = (u64)H0;
char* H1 = (char *)mmap(NULL,339968, PROT_READ|PROT_WRITE, 0);
H_Addr[1] = (u64)H1;
char* H2 = (char *)mmap(NULL,192512, PROT_READ|PROT_WRITE, 0);
H_Addr[2] = (u64)H2;
char* H3 = (char *)mmap(NULL,36864, PROT_READ|PROT_WRITE, 0);
H_Addr[3] = (u64)H3;
char* H4 = (char *)mmap(NULL,262144, PROT_READ|PROT_WRITE, 0);
H_Addr[4] = (u64)H4;
char* H5 = (char *)mmap(NULL,268173312, PROT_READ|PROT_WRITE, 0);
H_Addr[5] = (u64)H5;
char* H6 = (char *)mmap(NULL,17907712, PROT_READ|PROT_WRITE, 0);
H_Addr[6] = (u64)H6;
char* H7 = (char *)mmap(NULL,8192, PROT_READ|PROT_WRITE, 0);
H_Addr[7] = (u64)H7;
char* H8 = (char *)mmap(NULL,15376384, PROT_READ|PROT_WRITE, 0);
H_Addr[8] = (u64)H8;
char* H9 = (char *)mmap(NULL,12288, PROT_READ|PROT_WRITE, 0);
H_Addr[9] = (u64)H9;
char* H10 = (char *)mmap(NULL,249856, PROT_READ|PROT_WRITE, 0);
H_Addr[10] = (u64)H10;
char* H11 = (char *)mmap(NULL,2097152, PROT_READ|PROT_WRITE, 0);
H_Addr[11] = (u64)H11;
char* H12 = (char *)mmap(NULL,167936, PROT_READ|PROT_WRITE, 0);
H_Addr[12] = (u64)H12;
char* H13 = (char *)mmap(NULL,2093056, PROT_READ|PROT_WRITE, 0);
H_Addr[13] = (u64)H13;
char* H14 = (char *)mmap(NULL,2093056, PROT_READ|PROT_WRITE, 0);
H_Addr[14] = (u64)H14;
char* H15 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[15] = (u64)H15;
char* H16 = (char *)mmap(NULL,2093056, PROT_READ|PROT_WRITE, 0);
H_Addr[16] = (u64)H16;
char* H17 = (char *)mmap(NULL,2097152, PROT_READ|PROT_WRITE, 0);
H_Addr[17] = (u64)H17;
char* H18 = (char *)mmap(NULL,315392, PROT_READ|PROT_WRITE, 0);
H_Addr[18] = (u64)H18;
char* H19 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[19] = (u64)H19;
char* H20 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[20] = (u64)H20;
char* H21 = (char *)mmap(NULL,77824, PROT_READ|PROT_WRITE, 0);
H_Addr[21] = (u64)H21;
char* H22 = (char *)mmap(NULL,2097152, PROT_READ|PROT_WRITE, 0);
H_Addr[22] = (u64)H22;
char* H23 = (char *)mmap(NULL,16384, PROT_READ|PROT_WRITE, 0);
H_Addr[23] = (u64)H23;
char* H24 = (char *)mmap(NULL,2097152, PROT_READ|PROT_WRITE, 0);
H_Addr[24] = (u64)H24;
char* H25 = (char *)mmap(NULL,856064, PROT_READ|PROT_WRITE, 0);
H_Addr[25] = (u64)H25;
char* H26 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[26] = (u64)H26;
char* H27 = (char *)mmap(NULL,8192, PROT_READ|PROT_WRITE, 0);
H_Addr[27] = (u64)H27;
char* H28 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[28] = (u64)H28;
char* H29 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[29] = (u64)H29;
char* H30 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[30] = (u64)H30;
char* H31 = (char *)mmap(NULL,12288, PROT_READ|PROT_WRITE, 0);
H_Addr[31] = (u64)H31;
char* H32 = (char *)mmap(NULL,16384, PROT_READ|PROT_WRITE, 0);
H_Addr[32] = (u64)H32;
char* H33 = (char *)mmap(NULL,159744, PROT_READ|PROT_WRITE, 0);
H_Addr[33] = (u64)H33;
char* H34 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[34] = (u64)H34;
char* H35 = (char *)mmap(NULL,8437760, PROT_READ|PROT_WRITE, 0);
H_Addr[35] = (u64)H35;
char* H36 = (char *)mmap(NULL,28672, PROT_READ|PROT_WRITE, 0);
H_Addr[36] = (u64)H36;
char* H37 = (char *)mmap(NULL,12288, PROT_READ|PROT_WRITE, 0);
H_Addr[37] = (u64)H37;
char* H38 = (char *)mmap(NULL,28672, PROT_READ|PROT_WRITE, 0);
H_Addr[38] = (u64)H38;
char* H39 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[39] = (u64)H39;
char* H40 = (char *)mmap(NULL,8192, PROT_READ|PROT_WRITE, 0);
H_Addr[40] = (u64)H40;
char* H41 = (char *)mmap(NULL,8192, PROT_READ|PROT_WRITE, 0);
H_Addr[41] = (u64)H41;
char* H42 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[42] = (u64)H42;
char* H43 = (char *)mmap(NULL,4096, PROT_READ|PROT_WRITE, 0);
H_Addr[43] = (u64)H43;
char S0[135168];
S_Addr[0] = (u64)S0;

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
    