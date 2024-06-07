#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pin.H"

FILE* trace;
unsigned long start_stack = 0;
unsigned long end_stack = 0;


// Print a memory read record
VOID RecordMemRead(VOID* ip, VOID* addr) { fprintf(trace, "%p: R %p\n", ip, addr); }

// Print a memory write record
VOID RecordMemWrite(VOID* ip, VOID* addr,ADDRINT wradd, UINT32 mem_write_size) {
    fprintf(trace, "%p: W %p %lx size: %u %u\n", ip, addr, wradd, mem_write_size, PIN_GetTid());
    
    /*if(start_stack!=0 && end_stack!=0 && (start_stack <= (unsigned long)addr) && ((unsigned long)addr <= end_stack)){
    fprintf(trace, "%p: W %p size: %u\n", ip, addr, mem_write_size);
    }*/ }


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
       // UINT32 refSize = INS_MemoryOperandSize(ins, memOp);
       // UINT32 tid = PIN_GetTid();

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
                    IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_END);

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
    reaid(fifo_fd,&start_stack,sizeof(unsigned long));
    read(fifo_fd,&end_stack,sizeof(unsigned long));
    printf("start:%lx, end:%lx\n",start_stack, end_stack);*/
    trace = fopen("pinatrace.out", "w");
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

