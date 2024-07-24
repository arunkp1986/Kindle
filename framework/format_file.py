import csv
import sys
from pathlib import Path

if(len(sys.argv)<2):
    print("pass trace file name")
    exit()

filename = sys.argv[1]
usec_in_sec = 1000000
usec_in_msec = 1000
count = 0
out = {}

va_mapping = {'H':{},'S':{}}

scount = 0
hcount = 0
record_count = 0

def read_maps(filename):
    global hcount
    global scount
    with open(filename) as fmaps:
        for line in fmaps:
            l = line.split()
            if(len(l) > 5 and 'stack' in l[5]):
                #print(l)
                addr = l[0].split('-')
                start = int(addr[0], 16)
                end = int(addr[1], 16)
                key = start>>12
                label = 'S'+str(scount)
                size = end-start
                scount += 1
                va_mapping['S'].update({key:{'start':start,'end':end,'label':label,'size':size}})
            if(not int(l[4]) and len(l)==5):
                #print(l)
                addr = l[0].split('-')
                start = int(addr[0], 16)
                end = int(addr[1], 16)
                key = start>>12
                label = 'H'+str(hcount)
                size = end-start
                hcount += 1
                va_mapping['H'].update({key:{'start':start,'end':end,'label':label,'size':size}})
                #print(start,end,key)
    #print(va_mapping)
"""
only Anonymous Heap areas and stack area are written to data file
"""
def read_trace(tracefile):
    flag = 0
    count = 0
    start_sec = 0
    start_usec = 0
    global record_count
    try:
        fd = open(tracefile,"r")
    except FileNotFoundError:
        print("{0} not present".format(filename))
        exit()
    
    with open('data.csv','w',newline='')as csvfile:
        fieldnames = ['interval','offset','ops','size','type','index']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        #writer.writeheader()
        for line in fd:
            mod_line = line.rstrip("\n") 
            l = mod_line.split(" ")
            try:
                sec = int(l[4], base=10)
            except:
                print("exception in second read", l[4])
                exit()
            try:
                usec = int(l[5], base=10)
            except:
                print("exception in microsecond read", l[5])
                exit()
            size = int(l[3], base=10)
            if(flag == 0):
                #print("only once")
                start_usec = sec*1000000+usec
                flag = 1
            elapsed_msec = int(((sec*1000000+usec)-start_usec)/usec_in_msec)
            op = l[0]
            va = int(l[2],16)
            area = 'X'
            offset = -1
            for k1,v1 in va_mapping['H'].items():
                if(va>>12 >= k1):
                    if(v1['start'] <= va and va < v1['end']):
                        offset = va-v1['start']
                        area = v1['label']
                        index = int(v1['label'][1:],10)
            for k1,v1 in va_mapping['S'].items():
                if(va>>12 >= k1):
                    if(v1['start'] <= va and va < v1['end']):
                        offset = va-v1['start']
                        area = v1['label']
                        index = int(v1['label'][1:],10)
            #assert area != 'X' and offset !=-1,"area and offset are wrong"
            if(area != 'X' or offset !=-1):
                writer.writerow({'interval':elapsed_msec,'offset':offset,'ops':op,'size':size,'type':area,'index':index})
                record_count+=1
            #if(count == recordcount):
             #   break;
            #count+=1
            


def generate_code(mapping):
    s = """#include<ulib.h>
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
"""   
    s +="u64 H_Addr[{0}];\n".format(hcount)
    s +="u64 S_Addr[{0}];\n".format(scount)
    lhcount=0
    lscount=0
    for k1,v1 in va_mapping.items():
        if(k1 == 'H'):
            for k2,v2 in v1.items():
                s+="char* {0} = (char *)mmap(NULL,{1}, PROT_READ|PROT_WRITE, 0);\n".format(v2['label'],v2['size'])
                s +="H_Addr[{0}] = (u64){1};\n".format(lhcount,v2['label'])
                lhcount += 1
        elif(k1 == 'S'):
            for k2,v2 in v1.items():
                s+="char {0}[{1}];\n".format(v2['label'],v2['size'])
                s +="S_Addr[{0}] = (u64){1};\n".format(lscount,v2['label'])
                lscount += 1
    s += r"""
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
    """
    init_file = open('init.c', 'w')
    init_file.write(s)

if __name__ == '__main__':
    read_maps("maps")
    read_trace(filename)
    generate_code(va_mapping)
    print(record_count)
