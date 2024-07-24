#include <nonvolatile.h>
#include <memory.h>
#include <context.h>
#include <lib.h>
#include <entry.h>
#include <list.h>


struct list_head * saved_state_list;
//LIST_HEAD(saved_state_list);

void init_saved_state_list(){
    init_list_head(saved_state_list);
    clflush_multiline((u64)saved_state_list,sizeof(struct list_head));
}

inline void clflush_multiline(u64 address, u32 size){
    u64 end_addr = address+size;
    u64 start_addr = ((address>>6)<<6);
    //start_addr = start_addr-64;
    asm volatile("sfence");
    while(start_addr < end_addr){
        void * fl_addr = (void*)(start_addr);
        asm volatile ("clflush (%0)"
			:: "r"(fl_addr)
			:"memory");
	start_addr += 64;
    }
    asm volatile("sfence");
}

/*This gives the saved persistent state of a process
 *Two contexts; one dirty, the other consistent alternatively. 
 *The consistent one is denoted by latest;
 *In which case log_status is -1;
*/
struct saved_state *create_saved_state(struct exec_context *ctx) {
    struct saved_state *ss = (struct saved_state *)os_chunk_alloc(sizeof(struct saved_state),NVM_META_REG);
    if(!ss){
        printk("Error creating saved state\n");
	return NULL;
    }
    ss->latest = -1;  
    ss->state = INVALID; 
    ss->pid = ctx->pid;
    dprintk("create saved state pid:%u\n",ctx->pid);
    ss->context[0] =  (struct exec_context *)os_chunk_alloc(sizeof(struct exec_context), NVM_META_REG);

    if (!ss->context[0]) {
        printk("Something is wrong\n");
    }
    ss->context[1] = (struct exec_context *)os_chunk_alloc(sizeof(struct exec_context), NVM_META_REG);
    
    if (!ss->context[1]) {
        printk("Something is wrong\n");
    }
    init_list_head(&ss->log);
    init_list_head(&ss->nv_mappings);    
    init_list_head(&ss->nv_stack);
    list_add(&ss->list,saved_state_list);

    clflush_multiline((u64)&ss->log, sizeof(struct list_head));
    clflush_multiline((u64)&ss->nv_mappings, sizeof(struct list_head));
    clflush_multiline((u64)&ss->nv_stack, sizeof(struct list_head));
    clflush_multiline((u64)&ss->list, sizeof(struct list_head));
    clflush_multiline((u64)ss, sizeof(struct saved_state));
    clflush_multiline((u64)saved_state_list,sizeof(struct list_head));
    return ss;
}

struct saved_state *get_saved_state(struct exec_context *ctx) {
    struct saved_state *ss;
    struct list_head* temp;
    dprintk("get saved state pid:%u, saved_state_list:%x\n",ctx->pid,saved_state_list);
    list_for_each(temp,saved_state_list){
        ss = list_entry(temp, struct saved_state, list);
	dprintk("ss pid:%u\n",ss->pid);
        if ( ss && ss->pid==ctx->pid){
            return ss;
	}
    }
    return NULL;
}

void make_nv_mappings(struct exec_context *ctx, u64 vaddr, u32 access_flags){
    struct list_head* temp;
    struct list_head* head;
    struct mapping_level_one * ml;
    struct mapping_entry* me;
    struct saved_state *cs = get_saved_state(ctx);
    u64* pte = get_user_pte(ctx, vaddr, 0);
    if(pte){
        head = &cs->nv_mappings;
        temp = head->next;
        while( temp != head ){
            ml = list_entry(temp, struct mapping_level_one, list);
            if( ml->addr == (vaddr>>21) ){
                me = (struct mapping_entry*)os_chunk_alloc(sizeof(struct mapping_entry), NVM_META_REG);
                me->vaddr = vaddr>>PAGE_SHIFT;
                me->nvaddr = (*pte)>>PTE_SHIFT;
                me->access_flags = access_flags;
                list_add_tail(&me->list, &ml->me);
                clflush_multiline((u64)me, sizeof(struct mapping_entry));
                clflush_multiline((u64)&ml->me, sizeof(struct list_head));
	        return;
	    }
	    temp = temp->next;
	}
        ml = (struct mapping_level_one*)os_chunk_alloc(sizeof(struct mapping_level_one), NVM_META_REG);
	ml->addr = (vaddr>>21);
	me = (struct mapping_entry*)os_chunk_alloc(sizeof(struct mapping_entry), NVM_META_REG);
        me->vaddr = vaddr>>PAGE_SHIFT;
        me->nvaddr = (*pte)>>PTE_SHIFT;
        me->access_flags = access_flags;
	init_list_head(&ml->me);
        list_add_tail(&me->list, &ml->me);
        list_add_tail(&ml->list, &cs->nv_mappings);
        clflush_multiline((u64)ml, sizeof(struct mapping_level_one));
        clflush_multiline((u64)me, sizeof(struct mapping_entry));
        clflush_multiline((u64)&cs->nv_mappings, sizeof(struct list_head));
    }
}

u8 is_mapping_present(struct saved_state *cs, u64 vaddr){
    struct mapping_entry * me;
    struct mapping_level_one * ml;
    struct list_head* temp;
    struct list_head* head;
    struct list_head* me_temp;
    struct list_head* me_head;
    head = &cs->nv_mappings;
    temp = head->next;
    while( temp != head ){
        ml = list_entry(temp, struct mapping_level_one, list);
        if( ml->addr == (vaddr>>21) ){
            me_head = &ml->me;
	    me_temp = me_head->next;
	    dprintk("ml addr:%lx,head:%lx,temp:%lx\n",ml->addr,(u64)me_head,(u64)me_temp);
	    while(me_temp != me_head){
	        me = list_entry(me_temp, struct mapping_entry, list);
		if(me->vaddr == (vaddr>>PAGE_SHIFT) ){
                     return 1;
		}
	        me_temp = me_temp->next;
	    }
	 }
	 temp = temp->next;
     }
    return 0;
}

struct mapping_entry * get_nv_mapping(struct saved_state *cs, u64 vaddr){
    struct mapping_entry * me;
    struct mapping_level_one * ml;
    struct list_head* temp;
    struct list_head* head;
    struct list_head* me_temp;
    struct list_head* me_head;
    head = &cs->nv_mappings;
    temp = head->next;
    while( temp != head ){
        ml = list_entry(temp, struct mapping_level_one, list);
        if( ml->addr == (vaddr>>21) ){
            me_head = &ml->me;
	    me_temp = me_head->next;
	    while(me_temp != me_head){
	        me = list_entry(me_temp, struct mapping_entry, list);
		if(me->vaddr == (vaddr>>PAGE_SHIFT) ){
                     return me;
		}
	        me_temp = me_temp->next;
	    }
	 }
	 temp = temp->next;
     }
    return NULL;
}


