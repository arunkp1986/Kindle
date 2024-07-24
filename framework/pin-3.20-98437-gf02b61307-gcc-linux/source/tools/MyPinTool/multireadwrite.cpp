#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unordered_map>
#include "pin.H"

#define NUM_THREADS 64

FILE* trace;
PIN_LOCK pinLock;

struct sdata{
    unsigned pid;
    unsigned long start;
    unsigned long end;
};

std::unordered_map<unsigned,struct sdata*> ht;

int loop_index_read;
int loop_index_write;
int found;

VOID ThreadStart(THREADID threadid, CONTEXT* ctxt, INT32 flags, VOID* v)
{
    PIN_GetLock(&pinLock, threadid + 1);
    printf("thread begin %d\n", threadid);
    //fflush(trace);
    PIN_ReleaseLock(&pinLock);
}

// Print a memory read record
VOID RecordMemRead(VOID* ip, VOID* addr, UINT32 mem_read_size, THREADID threadid) { 
    char filename[50];
    unsigned long start;
    unsigned long end;
    struct timeval tm;
    int sysfs_fd;
    int read_ret;
    struct sdata * temp_sdata;
    unsigned tid = PIN_GetTid();
    //printf("application threadid : %u, system tid: %u\n",threadid, tid);
    PIN_GetLock(&pinLock, threadid + 1);
check:
    if(ht.find(tid) == ht.end()){
        printf("read from pin: tid: %u\n",tid);
        //fprintf(stderr,"from pin: tid: %u\n",tid);
	//fflush(stderr);
	loop_index_read = found;
        for(; loop_index_read<=NUM_THREADS; loop_index_read++){
	    //printf("found : %u loop_index_read:%u\n",found ,loop_index_read);
            sprintf(filename,"/sys/kernel/kobject_example/%u",loop_index_read);
	    if(access(filename, F_OK) == 0){ //checking for the availability of sysfs file.
                sysfs_fd = open(filename, O_RDWR);
	        printf(" read from pin: filename: %s, sysfs_fd: %d\n",filename,sysfs_fd);
	        //fprintf(stderr,"from pin: filename: %s, sysfs_fd: %d\n",filename,sysfs_fd);
		//fflush(stderr);
	        temp_sdata = (struct sdata *)malloc(sizeof(struct sdata));
	        memset(temp_sdata,0,sizeof(struct sdata));	
                read_ret = read(sysfs_fd,temp_sdata,sizeof(struct sdata));
		if(!read_ret){ perror("sysfs read");}
	        printf(" read pid: %u, start: %lx end:%lx\n",temp_sdata->pid,temp_sdata->start,temp_sdata->end);
	        //fprintf(stderr,"pid: %u, start: %lx end:%lx\n",temp_sdata->pid,temp_sdata->start,temp_sdata->end);
		//fflush(stderr);
		if((temp_sdata->pid != 0) && (ht.find(temp_sdata->pid) == ht.end())){
		    printf("inside ht insert, pid:%u\n",temp_sdata->pid);
		    //fprintf(stderr,"inside ht insert, pid:%u\n",temp_sdata->pid);
		    //fflush(stderr);
		    ht[temp_sdata->pid] = (struct sdata *)malloc(sizeof(struct sdata));
		    ht[temp_sdata->pid]->pid = temp_sdata->pid;
	            ht[temp_sdata->pid]->start = temp_sdata->start;
		    ht[temp_sdata->pid]->end = temp_sdata->end;
	        }
	    found += 1;
	    }
        }
    }
    PIN_ReleaseLock(&pinLock);
    //printf("from pin: after lock tid: %u\n",tid);
    if(ht.find(tid) == ht.end()){
	 //printf("could not find for tid %d\n", tid) ;  
	 //fprintf(stderr, "could not find for tid %d\n", tid) ;  
	 //fflush(stderr);
	 //sleep(2);
         goto check;
    }
    for( auto it = ht.begin(); it != ht.end(); ++it){
        start = (unsigned long)(it->second)->start;
        end = (unsigned long)(it->second)->end; 
        //printf("from hmap tid: %u addr: %lx, start: %lx end:%lx\n",tid,(unsigned long)addr,start,end); 
       if((start <= (unsigned long)addr) && ((unsigned long)addr <= end)){
	    //printf("in PIN stack area application_threadid:%u, system tid:%u\n",threadid, tid);
	    //fflush(stderr);
            if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
            PIN_GetLock(&pinLock, threadid + 1);
	    fprintf(trace, "R S %u %p %u %ld %ld\n",tid, addr, mem_read_size, tm.tv_sec,tm.tv_usec);
	    fflush(trace);
            PIN_ReleaseLock(&pinLock);
	    return; //got the stack area, now come out of loop
       }
    }
    if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(trace, "R H %u %p %u %ld %ld\n",tid, addr, mem_read_size, tm.tv_sec,tm.tv_usec);
    fflush(trace);
    PIN_ReleaseLock(&pinLock);
}

// Print a memory write record
VOID RecordMemWrite(VOID* ip, VOID* addr, UINT32 mem_write_size, THREADID threadid) {
    char filename[50];
    unsigned long start;
    unsigned long end;
    struct timeval tm;
    int sysfs_fd;
    int read_ret;
    struct sdata * temp_sdata;
    unsigned tid = PIN_GetTid();
    //printf("application threadid : %u, system tid: %u\n",threadid, tid);
    PIN_GetLock(&pinLock, threadid + 1);
check:
    if(ht.find(tid) == ht.end()){
        printf(" write from pin: tid: %u\n",tid);
	//fflush(stderr);
	loop_index_write = found;
        for(; loop_index_write<=NUM_THREADS;loop_index_write++){
	    //printf("found :%u loop_index_write:%u\n",found, loop_index_write);
            sprintf(filename,"/sys/kernel/kobject_example/%u",loop_index_write);
	    if(access(filename, F_OK) == 0){ //checking for the availability of sysfs file.
                sysfs_fd = open(filename, O_RDWR);
	        printf("write from pin: filename: %s, sysfs_fd: %d\n",filename,sysfs_fd);
	        //fprintf(stderr,"from pin: filename: %s, sysfs_fd: %d\n",filename,sysfs_fd);
		//fflush(stderr);
	        temp_sdata = (struct sdata *)malloc(sizeof(struct sdata));
	        memset(temp_sdata,0,sizeof(struct sdata));	
                read_ret = read(sysfs_fd,temp_sdata,sizeof(struct sdata));
		if(!read_ret){ perror("sysfs read");}
	        printf("write pid: %u, start: %lx end:%lx\n",temp_sdata->pid,temp_sdata->start,temp_sdata->end);
	        //fprintf(stderr,"pid: %u, start: %lx end:%lx\n",temp_sdata->pid,temp_sdata->start,temp_sdata->end);
		//fflush(stderr);
		if((temp_sdata->pid != 0) && (ht.find(temp_sdata->pid) == ht.end())){
		    printf("inside ht insert, pid:%u\n",temp_sdata->pid);
		    //fprintf(stderr,"inside ht insert, pid:%u\n",temp_sdata->pid);
		    //fflush(stderr);
		    ht[temp_sdata->pid] = (struct sdata *)malloc(sizeof(struct sdata));
		    ht[temp_sdata->pid]->pid = temp_sdata->pid;
	            ht[temp_sdata->pid]->start = temp_sdata->start;
		    ht[temp_sdata->pid]->end = temp_sdata->end;
	        }
	    found += 1;
	    }
        }
    }
    PIN_ReleaseLock(&pinLock);
    //printf("from pin: after lock tid: %u\n",tid);
    if(ht.find(tid) == ht.end()){
	 //fprintf(stderr, "could not find for tid %d\n", tid) ;  
	 //fflush(stderr);
	 //sleep(2);
         goto check;
    }
    for( auto it = ht.begin(); it != ht.end(); ++it){
        start = (unsigned long)(it->second)->start;
        end = (unsigned long)(it->second)->end; 
        //printf("from hmap tid: %u addr: %lx, start: %lx end:%lx\n",tid,(unsigned long)addr,start,end); 
       if((start <= (unsigned long)addr) && ((unsigned long)addr <= end)){
	    //fprintf(stderr,"in PIN stack area application_threadid:%u, system tid:%u\n",threadid, tid);
	    //fflush(stderr);
            if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
            PIN_GetLock(&pinLock, threadid + 1);
	    fprintf(trace, "W S %u %p %u %ld %ld\n",tid, addr, mem_write_size, tm.tv_sec,tm.tv_usec);
	    fflush(trace);
            PIN_ReleaseLock(&pinLock);
	    return; //got the stack area, now come out of loop
       }
    }
    if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(trace, "W H %u %p %u %ld %ld\n",tid, addr, mem_write_size, tm.tv_sec,tm.tv_usec);
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
   // Initialize the pin lock
    PIN_InitLock(&pinLock);

    if (PIN_Init(argc, argv)) return Usage();
    PIN_InitSymbols();
    trace = fopen("pinatrace.out", "w");
    loop_index_read = 0;
    loop_index_write = 0;
    found = 0;
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

