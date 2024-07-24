#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include "pin.H"

#define NUM_THREADS 8

FILE* trace;
unsigned long start_stack = 0;
unsigned long end_stack = 0;

struct sysfs_data{
    unsigned long start;
    unsigned long end;
};



struct sysfs_data *sysfs_array[NUM_THREADS+1] = {NULL};

// Print a memory read record
VOID RecordMemRead(VOID* ip, VOID* addr) { fprintf(trace, "%p: R %p\n", ip, addr); }

// Print a memory write record
VOID RecordMemWrite(VOID* ip, VOID* addr, UINT32 mem_write_size) {
    int loop_index = 0;
    char filename[50];
    unsigned long start;
    unsigned long end;
    struct timeval tm;
    int sysfs_fd;
    int read_ret;
    int flag = 0;
    struct sysfs_data **temp = &sysfs_array[0];
    while(*temp!=NULL){ /*check address against all recorded stack start and end values*/
        start = (*temp)->start;
	end   = (*temp)->end;
        if((start <= (unsigned long)addr) && ((unsigned long)addr <= end)){
	    if (gettimeofday(&tm, NULL) == -1){
	        perror("clock gettime");
	    }
            fprintf(trace, "%p %u %ld %ld\n", addr, mem_write_size, tm.tv_sec,tm.tv_usec);
	    flag = 1;
	    break;
	    }
        loop_index++;
	if(loop_index == (NUM_THREADS+1)){ break;}
        temp++;
    }

    if(!flag && loop_index != (NUM_THREADS+1)){  /* came out of while because of empty sysfs_array position, read file to fill the pos*/
        sprintf(filename,"/sys/kernel/kobject_example/%u",loop_index);
        if(access(filename, F_OK) == 0){ /*checking for the availability of sysfs file.*/
            sysfs_fd = open(filename, O_RDWR);
	    printf("sysfs_fd:%d\n",sysfs_fd);
	    sysfs_array[loop_index] = (struct sysfs_data *)malloc(sizeof(struct sysfs_data));
            read_ret = read(sysfs_fd,sysfs_array[loop_index],sizeof(struct sysfs_data));
            if(read_ret == 0){
                perror("sysfs read");
                exit(-1);
            }
            //printf("from pin tool start:%lx",sysfs_array[loop_index]->start);
            start = sysfs_array[loop_index]->start;
	    end   = sysfs_array[loop_index]->end;
            if((start <= (unsigned long)addr) && ((unsigned long)addr <= end)){
                if (gettimeofday(&tm,NULL) == -1){
	            perror("clock gettime");}
		//address, size, time in seconds, time in usec
		fprintf(trace, "%p %u %ld %ld\n",addr, mem_write_size, tm.tv_sec,tm.tv_usec);
		}
	    }
        } 
 }


// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID* v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
      /*  if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
                                     IARG_END);
        }*/
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
            {
               
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
                    IARG_MEMORYWRITE_SIZE, IARG_END);

            /*if(start_stack!=0 && end_stack!=0 && (start_stack <= (unsigned long)IARG_MEMORYOP_EA) && ((unsigned long)IARG_MEMORYOP_EA <= end_stack)){
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
                                     IARG_MEMORYWRITE_SIZE, IARG_END);
            } */       
        }
    }
}

VOID Fini(INT32 code, VOID* v)
{
    fprintf(trace, "#eof\n");
    fclose(trace);

}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of memory addresses\n" + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char* argv[])
{

    if (PIN_Init(argc, argv)) return Usage();
    
    /*int fifo_fd = open("/tmp/myfifo",O_RDONLY);
    if(fifo_fd < 0){
        perror("fifo_fd:");
        exit(-1);
    }
    printf("before read\n");
    read(fifo_fd,&start_stack,sizeof(unsigned long));
    read(fifo_fd,&end_stack,sizeof(unsigned long));
    printf("start:%lx, end:%lx\n",start_stack, end_stack);*/
    trace = fopen("pinatrace.out", "w");
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

