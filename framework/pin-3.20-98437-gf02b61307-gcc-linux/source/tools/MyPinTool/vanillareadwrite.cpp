#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unordered_map>
#include "pin.H"

#define NUM_THREADS 8

FILE* trace;
PIN_LOCK pinLock;

VOID ThreadStart(THREADID threadid, CONTEXT* ctxt, INT32 flags, VOID* v)
{
    PIN_GetLock(&pinLock, threadid + 1);
    printf("thread begin %d\n", threadid);
    //fflush(trace);
    PIN_ReleaseLock(&pinLock);
}

// Print a memory read record
VOID RecordMemRead(VOID* ip, VOID* addr, UINT32 mem_read_size, THREADID threadid) { 
    struct timeval tm;
    unsigned tid = PIN_GetTid();
    //printf("application threadid : %u, system tid: %u\n",threadid, tid);
    if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(trace, "R %u %p %u %ld %ld\n",tid, addr, mem_read_size, tm.tv_sec,tm.tv_usec);
    fflush(trace);
    PIN_ReleaseLock(&pinLock);
}

// Print a memory write record
VOID RecordMemWrite(VOID* ip, VOID* addr, UINT32 mem_write_size, THREADID threadid) {
    unsigned tid = PIN_GetTid();
    struct timeval tm;
    //printf("application threadid : %u, system tid: %u\n",threadid, tid);
    if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(trace, "W %u %p %u %ld %ld\n",tid, addr, mem_write_size, tm.tv_sec,tm.tv_usec);
    fflush(trace);
    PIN_ReleaseLock(&pinLock);
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
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
			    IARG_MEMORYREAD_SIZE,IARG_THREAD_ID,IARG_END);
        }
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
            {
               
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
                    IARG_MEMORYWRITE_SIZE, IARG_THREAD_ID, IARG_END);      
        }
    }
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT* ctxt, INT32 code, VOID* v)
{
    PIN_GetLock(&pinLock, threadid + 1);
    printf("thread end %d\n", threadid);
    //fflush(trace);
    PIN_ReleaseLock(&pinLock);
}


VOID Fini(INT32 code, VOID* v)
{
    //fprintf(trace, "#eof\n");
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
   // Initialize the pin lock
    PIN_InitLock(&pinLock);

    if (PIN_Init(argc, argv)) return Usage();
    PIN_InitSymbols();
    trace = fopen("pinatrace.out", "w");
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

