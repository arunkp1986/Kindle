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

static unsigned index = 0;
static unsigned long start_offset[3] = {1300000000,2300000000,3100000000}; //GAP PR
static unsigned long end_offset[3] = {1400000000,2400000000,3200000000}; //GAP PR
static UINT64 icount = 0;
static VOID* prev_ip = NULL;
VOID docount(VOID* ip, THREADID threadid){    
    PIN_GetLock(&pinLock, threadid + 1);
    if(!prev_ip){
        icount++;
        prev_ip = ip;
    }
    if(prev_ip != ip){
        icount++;
	prev_ip = ip;
    }
    if(index<2 && (icount == end_offset[index])){
        index += 1;
    }
    PIN_ReleaseLock(&pinLock);
}

// Print a memory read record
VOID RecordMemRead(CONTEXT* ctxt,VOID* ip, VOID* addr, UINT32 mem_read_size, THREADID threadid) {
    char filename[50];
    unsigned long start;
    unsigned long end;
    struct timeval tm;
    int sysfs_fd;
    int read_ret;
    struct sdata * temp_sdata;
    unsigned tid = PIN_GetTid();
    unsigned long rsp_val;
    assert(index<3);
    UINT64 start_ins = start_offset[index]; //Simpoint interval 100,000,000
    UINT64 end_ins = end_offset[index]; //considering 100,000,000 ins for tracing
    //UINT64 start_ins = 140300000000; //Simpoint interval 100,000,000 SSSP
    //UINT64 end_ins = 140400000000; //considering 100,000,000 ins for tracing SSSP

    docount(addr, threadid);
    if(icount >= 3200000000)
        return;
    //printf("application threadid : %u, system tid: %u\n",threadid, tid);
    PIN_GetLock(&pinLock, threadid + 1);
    //printf("got it\n");
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
	    PIN_GetContextRegval(ctxt, REG_RSP, (UINT8*)&rsp_val);
            if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
            if(start_ins <= icount && icount<end_ins){
                PIN_GetLock(&pinLock, threadid + 1);
	        fprintf(trace, "R S %u %p %u %ld %ld %lx\n",tid, addr, mem_read_size, tm.tv_sec,tm.tv_usec,rsp_val);
	        fflush(trace);
                PIN_ReleaseLock(&pinLock);
	    }
	    return; //got the stack area, now come out of loop
       }
    }
    if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
    if(start_ins <= icount && icount<end_ins){
        PIN_GetContextRegval(ctxt, REG_RSP, (UINT8*)&rsp_val);
        PIN_GetLock(&pinLock, threadid + 1);
        fprintf(trace, "R H %u %p %u %ld %ld %lx\n",tid, addr, mem_read_size, tm.tv_sec,tm.tv_usec,rsp_val);
        fflush(trace);
        PIN_ReleaseLock(&pinLock);
    }
}

// Print a memory write record
VOID RecordMemWrite(CONTEXT* ctxt, VOID* ip, VOID* addr, UINT32 mem_write_size, THREADID threadid) {
    char filename[50];
    unsigned long start;
    unsigned long end;
    struct timeval tm;
    int sysfs_fd;
    int read_ret;
    struct sdata * temp_sdata;
    unsigned tid = PIN_GetTid();
    unsigned long rsp_val;
    assert(index<3);
    UINT64 start_ins = start_offset[index]; //Simpoint interval 100,000,000
    UINT64 end_ins = end_offset[index]; //considering 100,000,000 ins for tracing
    //UINT64 start_ins = 140300000000; //Simpoint interval 100,000,000 sssp
    //UINT64 end_ins = 140400000000; //considering 100,000,000 ins for tracing sssp
    docount(addr,threadid);
    if(icount >= 3200000000)
        return;
    //printf("application threadid : %u, system tid: %u\n",threadid, tid);
    PIN_GetLock(&pinLock, threadid + 1);
    //printf("got it\n");
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
            if(start_ins <= icount && icount<end_ins){
	        PIN_GetContextRegval(ctxt, REG_RSP, (UINT8*)&rsp_val);
                PIN_GetLock(&pinLock, threadid + 1);
	        fprintf(trace, "W S %u %p %u %ld %ld %lx\n",tid, addr, mem_write_size, tm.tv_sec,tm.tv_usec,rsp_val);
	        fflush(trace);
                PIN_ReleaseLock(&pinLock);
	    }
	    return; //got the stack area, now come out of loop
       }
    }
    if (gettimeofday(&tm, NULL) == -1){perror("clock gettime");}
    if(start_ins <= icount && icount<end_ins){
        PIN_GetContextRegval(ctxt, REG_RSP, (UINT8*)&rsp_val);
        PIN_GetLock(&pinLock, threadid + 1);
        fprintf(trace, "W H %u %p %u %ld %ld %lx\n",tid, addr, mem_write_size, tm.tv_sec,tm.tv_usec,rsp_val);
        fflush(trace);
        PIN_ReleaseLock(&pinLock);
    }
}

VOID printip(VOID* ip) { 	
	fprintf(trace,"%p\n", ip); 

}
// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID* v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.
    //if(index < 1){
    //UINT64 start_ins = 505970000000; //Simpoint interval 10,000,000
    //UINT64 end_ins = 506070000000; //considering 100,000,000 ins

    //if(icount < start_ins){
	//printf("icount: %u\n",icount);
	//INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_INST_PTR, IARG_END);
    //}
    //else{
	//printf("icount:%lu,end_ins:%lu\n",icount,end_ins);	
        //if(icount < end_ins){
	    //printf("inside tracing ins\n");	
            UINT32 memOperands = INS_MemoryOperandCount(ins);
            // Iterate over each memory operand of the instruction.
	    //printf("memOperands:%u\n",memOperands);
	    if(!memOperands){
		//printf("calling docount\n");
	        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_INST_PTR, IARG_THREAD_ID, IARG_END);
	    }
	    else{
		//printf("calling memops\n");
            for (UINT32 memOp = 0; memOp < memOperands; memOp++){
                if (INS_MemoryOperandIsRead(ins, memOp)){
		    //printf("mem read\n");
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
				    IARG_CONTEXT, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp, 
				    IARG_MEMORYREAD_SIZE,IARG_THREAD_ID,IARG_END);
		}
               // Note that in some architectures a single memory operand can be
               // both read and written (for instance incl (%eax) on IA-32)
               // In that case we instrument it once for read and once for write.
               if (INS_MemoryOperandIsWritten(ins, memOp)){
		    //printf("mem write\n");
                   INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
				   IARG_CONTEXT, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
				   IARG_MEMORYWRITE_SIZE, IARG_THREAD_ID, IARG_END);
	       }
	    }
	    }
	//}
        //if(icount == end_ins){ index += 1; }
    //}
    //}
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

