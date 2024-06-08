#include <nonvolatile.h>
#include <memory.h>
#include <context.h>
#include <lib.h>
#include <entry.h>
#include <list.h>
#include <dirty.h>


struct list_head * clean_list = NULL;
struct list_head * dirty_list = NULL;
struct list_head *stat_list = NULL;
//struct list_head * processing_list = NULL;
struct list_head * saved_state_list;
//LIST_HEAD(saved_state_list);
u16 track_on = 1;
#define NUM_DRAM_PAGES 512 //2MB DRAM CACHE
#define MIGRATION_THRESHOLD 50

struct list_data{
    u64* pte;
    struct list_head list;
};


void init_saved_state_list(){
    init_list_head(saved_state_list);
    clflush_multiline((u64)saved_state_list,sizeof(struct list_head));
}

inline void clflush_multiline(u64 address, u32 size){
    u64 end_addr = address+size;
    u64 start_addr = (address&~((1<<6)-1));
    asm volatile("sfence":::"memory");
    while(start_addr < end_addr){
        void * fl_addr = (void*)(start_addr);
        asm volatile ("clwb %0;"
			:: "m"(fl_addr)
			:"memory");
	start_addr += 64;
    }
    asm volatile("sfence":::"memory");
}
unsigned used_pages = 0;
unsigned clean_used = 0;
void populate_list(){
   //printk("going to populate list\n");
   asm volatile("sfence":::"memory"); //to ensure pte modifications are visible
   struct exec_context *current = get_current_ctx();
   struct list_data * temp_data;
   struct list_head* temp_list;
   struct list_head* temp_cursor = NULL;
   struct list_data * cursor_data;
   u8 ret = 0;
   clean_used = 0;
   //update the dirty list to remove present mappings
   if(dirty_list && (dirty_list->next!=dirty_list)){
       temp_list = dirty_list->next;
       while(temp_list!=dirty_list){
           if(temp_cursor){
               cursor_data = list_entry(temp_cursor, struct list_data, list);
	       list_del(temp_cursor);
	       ret = os_chunk_free(NVM_META_REG,(u64)cursor_data,sizeof(struct list_data));
	       if(!ret){
	           printk("chunk free failed,%lx\n",(u64)cursor_data);
	       }
	   }
	   temp_cursor = temp_list;
	   temp_list = temp_list->next;
       }
	// to remove last entry
       //temp_cursor = temp_list;
       cursor_data = list_entry(temp_cursor, struct list_data, list);
       list_del(temp_cursor);
       ret = os_chunk_free(NVM_META_REG,(u64)cursor_data,sizeof(struct list_data));
       if(!ret){
           printk("chunk free failed,%lx\n",(u64)cursor_data);
       }
       //printk("checking dirty list empty\n");
       if(dirty_list->next!=dirty_list){
            printk("dirty, should not print this:%lx\n",(u64)dirty_list->next);
       }
   }
   temp_cursor = NULL;
   if(clean_list && (clean_list->next!=clean_list)){
       temp_list = clean_list->next;
        while(temp_list!=clean_list){
            if(temp_cursor){
	        cursor_data = list_entry(temp_cursor, struct list_data, list);
	        list_del(temp_cursor);
	        ret = os_chunk_free(NVM_META_REG,(u64)cursor_data,sizeof(struct list_data));
                if(!ret){
	           printk("chunk free failed,%lx\n", (u64)cursor_data);
		}
	    }
            temp_cursor = temp_list;
	    temp_list = temp_list->next;
	}
	// to remove last entry
	cursor_data = list_entry(temp_cursor, struct list_data, list);
	list_del(temp_cursor);
        ret = os_chunk_free(NVM_META_REG,(u64)cursor_data,sizeof(struct list_data));
        if(!ret){
	     printk("chunk free failed:%lx\n",(u64)cursor_data);
	}
        if(clean_list->next!=clean_list){
            printk("clean, should not print this,%lx\n",(u64)clean_list->next);
       }
   }
   if(!clean_list){
	printk("only once\n");
	clean_list = (struct list_head*)os_chunk_alloc(sizeof(struct list_head),NVM_META_REG); 
	//clean_list = (struct list_head*)os_page_alloc(FILE_STORE_REG); 
	//printk("clean list:%lx\n",(u64)clean_list);
	dirty_list = (struct list_head*)os_chunk_alloc(sizeof(struct list_head),NVM_META_REG);
	//dirty_list = (struct list_head*)os_page_alloc(FILE_STORE_REG);
	//printk("dirty list:%lx\n",(u64)dirty_list);
	stat_list = (struct list_head*)os_chunk_alloc(sizeof(struct list_head),NVM_META_REG);
	//printk("stat list:%lx,%u\n",(u64)stat_list,sizeof(struct list_head));
        init_list_head(clean_list);
        init_list_head(dirty_list);
	init_list_head(stat_list);
   }
        u32 clean_count = 0;
	u32 dirty_count = 0;
   
	u64 start = (current->mms)[MM_SEG_STACK].next_free;
        u64 end = (current->mms)[MM_SEG_STACK].end;
	//printk("print stats called start:%lx, end:%lx\n",start, end);
        for(u64 addr = start; addr < end; addr += PAGE_SIZE){
            u64* pte = get_user_pte(current, addr,0);
            u8 present = ((*pte&0x1)>0)?1:0;
	    u8 dirty = ((*pte&(1<<6))>0)?1:0;
	    u32 pfn = (*pte>>12)&(0xffffff);
            if(present && ((REGION_USER_START <= (u64)osmap(pfn)) 
				    && ((u64)osmap(pfn) < REGION_FILE_DS_START))){
	        //printk("USER_REG STACK\n");
	        if(dirty){
	            struct list_data * dt = (struct list_data*)os_chunk_alloc(sizeof(struct list_data),NVM_META_REG);
		    dt->pte = pte;
	            list_add_tail(&(dt->list),dirty_list);
		    dirty_count += 1;
	        }else{
	            struct list_data * dt = (struct list_data*)os_chunk_alloc(sizeof(struct list_data),NVM_META_REG); 
		    dt->pte = pte;
	            list_add_tail(&(dt->list),clean_list);
		    clean_count += 1;
		}
	    }
	}
	struct vm_area * vm = current->vm_area;
        while(vm){
            if(vm->is_nvm){
                for(u64 vaddr = vm->vm_start; vaddr < vm->vm_end; vaddr += PAGE_SIZE){
	            u64* pte = get_user_pte(current, vaddr,0);
                    u8 present = ((*pte&0x1)>0)?1:0;
	            u8 dirty = ((*pte&(1<<6))>0)?1:0;
	            u32 pfn = (*pte>>12)&(0xffffff);
                    if(present && ((REGION_USER_START <= (u64)osmap(pfn)) 
					    && ((u64)osmap(pfn) < REGION_FILE_DS_START))){
	                //printk("USER_REG HEAP \n");
	                if(dirty){
	                    struct list_data * dt = (struct list_data*)os_chunk_alloc(sizeof(struct list_data),NVM_META_REG);
		            dt->pte = pte;
	                    list_add_tail(&(dt->list),dirty_list);
		            dirty_count += 1;
			}else{
	                    struct list_data * dt = (struct list_data*)os_chunk_alloc(sizeof(struct list_data),NVM_META_REG); 
		            dt->pte = pte;
	                    list_add_tail(&(dt->list),clean_list);
		            clean_count += 1;
			}
		    }
		}
	    }
            vm = vm->vm_next;
	}
	used_pages = clean_count+dirty_count;
	//printk("used pages:%u, clean:%u, dirty:%u\n",used_pages,clean_count,dirty_count);
}

u32 stack_migrated_pages = 0;
u32 heap_migrated_pages = 0;
void process_address(u64 addr, u8 is_stack){
    extern u64* nvm_array;
    extern u64* dram_array;
    u64 mask = (u64)0xff<<52;
    struct exec_context *current = get_current_ctx();
    u64* pte = get_user_pte(current, addr,0);
    if(!pte){
        return;
    }
    clflush_multiline((u64)pte,sizeof(u64));
    //u8 accessed = ((*pte&(1<<5))>0)?1:0;
    u8 dirty = ((*pte&(1<<6))>0)?1:0;
    u8 present = ((*pte&0x1)>0)?1:0;
    u32 pfn = (*pte>>12)&(0xffffff);
    //printk("pfn:%lx\n",pfn);
    u32 access_count = (*pte&mask)>>52;
    if(present && 
             ((NVM_USER_REG_START <= (u64)osmap(pfn)) && ((u64)osmap(pfn) < REGION_NVM_ENDMEM))&&
	     access_count >= MIGRATION_THRESHOLD){
        //printk("candidate for migration\n");
	if(!dram_array || !nvm_array){
	    printk("Bug!!!%s\n",__func__);
	}
	//printk("access count:%u\n",access_count);
        u64 dram_pfn = 0;
        if(used_pages < NUM_DRAM_PAGES){
            dram_pfn = get_page(USER_REG);
	    used_pages += 1;
	    //printk("using free pages\n");
	}
	else{
            if(clean_list && clean_list->next != clean_list){
	        //use dram pfn from clean list and revert mapping in pte to nvm pfn
	        //printk("using dram pfn from clean list\n");
                struct list_head* temp_list = clean_list->next;
	        struct list_data * temp_data = (struct list_data*)list_entry(temp_list, struct list_data, list);
	        //printk("clean temp data:%lx\n",(unsigned long)temp_data);
	        u64* saved_pte = temp_data->pte;
	        //printk("saved pte:%lx\n",*saved_pte);
	        //u8 present = ((*pte&0x1)>0)?1:0;
	        //u8 dirty = ((*pte&(1<<6))>0)?1:0;
	        dram_pfn = ((*saved_pte)>>12)&(0xffffff);
	        //printk("dram pfn:%lx\n",dram_pfn);
                unsigned long dram_offset = ((((unsigned long) dram_pfn) << PAGE_SHIFT)-REGION_USER_START)>>PAGE_SHIFT;
	        u64* temp_dram = (u64*)((unsigned long)dram_array+(dram_offset*sizeof(struct mapping_entry*)));
	        struct mapping_entry * temp_mapen = (struct mapping_entry*) *temp_dram;
                u64 vaddr = temp_mapen->vaddr;
	        u64 nvm_pfn = temp_mapen->nvm; 
	        u16 flags = *saved_pte&0xfff;
                u64 new_pte = (*saved_pte>>36)<<36|nvm_pfn<<12|flags;
	        *saved_pte = new_pte;
	        *saved_pte = *saved_pte&~mask; //reset access count
	        list_del(temp_list);
	        asm volatile ("invlpg (%0);"
				   :: "r"(vaddr)
				   : "memory");
	    }
	    else if(dirty_list && dirty_list->next != dirty_list){
	        //clean list is empty, we need to use dirty dram page
	        //printk("using dram pfn from dirty list\n");
	        struct list_head* temp_list = dirty_list->next;
	        //printk("dirty temp list:%lx\n",(unsigned long)temp_list);
	        struct list_data * temp_data = (struct list_data*)list_entry(temp_list, struct list_data, list);
	        //printk("dirty temp data:%lx\n",(unsigned long)temp_data);
	        u64* saved_pte = temp_data->pte;
	        dram_pfn = (*saved_pte>>12)&(0xffffff);
	        //printk("dirty list dram pfn:%lx\n",dram_pfn);
	        unsigned long dram_offset = ((((unsigned long) dram_pfn) << PAGE_SHIFT)-REGION_USER_START)>>PAGE_SHIFT;
	        u64* temp_dram = (u64*)((unsigned long)dram_array+(dram_offset*sizeof(struct mapping_entry*)));
	        struct mapping_entry * temp_mapen = (struct mapping_entry*) *temp_dram;
	        u64 vaddr = temp_mapen->vaddr;
	        u64 nvm_pfn = temp_mapen->nvm; 
	        u64 *src_addr = (u64 *)osmap(dram_pfn);
                u64 *dest_addr = (u64 *)osmap(nvm_pfn);
                clflush_multiline(vaddr,4096);
	        //printk("src:%lx,dest:%lx\n",(u64)src_addr,(u64)dest_addr);
	        for(int i=0; i<512; i++){
	            *dest_addr = *src_addr;
		    dest_addr += 1;
		    src_addr += 1;
		}
                clflush_multiline((u64)dest_addr,4096);
	        u16 flags = *saved_pte&0xfff;
                u64 new_pte = (*saved_pte>>36)<<36|nvm_pfn<<12|flags;
	        *saved_pte = new_pte;
	        *saved_pte = *saved_pte&~mask; //reset access count 
	        *saved_pte = *saved_pte&~(1<<6); //reset dirty bit
	        list_del(temp_list);
	        //printk("dirty saved pte:%lx\n",*saved_pte);
	        asm volatile ("invlpg (%0);"
				   :: "r"(vaddr)
				   : "memory");
	       }else{
	            //printk("no clean or dirty dram page available\n");
	           //printk("going to print clean list\n");
	           return;
	       }
	}
	//printk("going to add entry in mapen\n");
	if(dram_pfn){
            if(is_stack){
	        stack_migrated_pages += 1;
	    }else{
                heap_migrated_pages += 1;
	    }
	    //printk("migration\n");
	    unsigned long nvm_offset = ((((unsigned long) pfn) << PAGE_SHIFT)-NVM_USER_REG_START)>>PAGE_SHIFT;
            struct mapping_entry * mapen = (struct mapping_entry*)os_chunk_alloc(sizeof(struct mapping_entry),NVM_META_REG);
            unsigned long dram_offset = ((((unsigned long) dram_pfn) << PAGE_SHIFT)-REGION_USER_START)>>PAGE_SHIFT;
	    mapen->vaddr = addr;
            mapen->dram = dram_pfn;
            mapen->nvm = pfn;
            u64* temp_nvm = (u64*)((unsigned long)nvm_array+(nvm_offset*sizeof(struct mapping_entry*)));
            //printk("dram_pfn:%x\n",dram_pfn);
            *temp_nvm = (unsigned long)mapen;
            u64* temp_dram = (u64*)((unsigned long)dram_array+(dram_offset*sizeof(struct mapping_entry*)));
            *temp_dram = (unsigned long)mapen;
            clflush_multiline(addr,4096);
	    //printk("going to copy page\n");
            u64 *src_addr = (u64*)osmap(pfn);
            u64 *dest_addr = (u64*)osmap(dram_pfn);
	    for(int i=0; i<512; i++){
	        *dest_addr = *src_addr;
	        dest_addr += 1;
	        src_addr += 1;
	    }
            clflush_multiline((u64)dest_addr,4096);
	    u16 flags = *pte&0xfff;
            u64 new_pte = (*pte>>36)<<36|dram_pfn<<12|flags;
	    //printk("new pte:%lx\n",new_pte);
	    //printk("going to update pte\n");
	    *pte = new_pte;
	    *pte = *pte&~mask; //reset access count 
	    *pte = *pte&~(1<<6); //reset dirty bit
	    //printk("addr:%lx, new pte:%lx\n",addr, *pte);
	    asm volatile ("invlpg (%0);"
                  :: "r"(addr)
                  : "memory");
	}
    }
    else if(access_count){
	//printk("old pte:%lx\n",*pte);
        *pte = *pte&~mask; //reset access count
	//printk("new pte:%lx\n",*pte);
        asm volatile ("invlpg (%0);"
                  :: "r"(addr)
                  : "memory");
    }
}

//This function is called to migrate pages at a fixed interval
//It checks access count in PTE >= threshold value,
//Mark the NVM page for migration if the access count is greater than threshold
//if the PTE contains a DRAM mapping, add it to clean and dirty list of pages
//if NVM migration does not find free DRAM page, pop a page from clean list
//if clean list is empty pop a page from dirty list and move it to NVM first
unsigned migration_num = 0;
int migrate_pages(){
    //printk("migrate pages called\n");
    migration_num += 1;
    stack_migrated_pages = 0;
    heap_migrated_pages = 0;
    struct exec_context *current = get_current_ctx();
    populate_list(); //may decrease used page count
    u64 start = (current->mms)[MM_SEG_STACK].next_free;
    u64 end = (current->mms)[MM_SEG_STACK].end;
    //inspect the stack area to migrate pages
    //printk("going to do stack\n");
    for(u64 vaddr=start; vaddr<end; vaddr+=PAGE_SIZE){
	    process_address(vaddr, 1);
    }
    //printk("going to do heap\n");
    //inspect the heap area to migrate pages
    struct vm_area * vm = current->vm_area;
    while(vm){
        if(vm->is_nvm){
            for(u64 vaddr = vm->vm_start; vaddr < vm->vm_end; vaddr += PAGE_SIZE){
	        process_address(vaddr, 0);
	    }
        }
        vm = vm->vm_next;
    }
    struct checkpoint_stats* chst = (struct checkpoint_stats*)os_chunk_alloc(sizeof(struct checkpoint_stats),NVM_META_REG);
    chst->num = migration_num;
    chst->stack_migrated_pages = stack_migrated_pages;
    chst->heap_migrated_pages = heap_migrated_pages;
    list_add_tail(&chst->list,stat_list);
    //printk("chst:%lx,%lx\n",(u64)chst, (u64)&chst->list);
    //printk("stat list:%lx,%lx\n",(u64)stat_list, (u64)stat_list->next);
    //printk("heap done\n");
    //printk("migrated stack:%u, heap:%u\n",stack_migrated_pages,heap_migrated_pages);
    //printk("-----done-----\n");
}


u8 print_checkpoint_stats(){
    struct checkpoint_stats* chst;
    struct list_head* temp;
    struct list_head* head;
    head = stat_list;
    temp = head->next;
    //printk("migration:%u\n",migration_num);
    //printk("stat list:%lx,%lx\n",(u64)stat_list, (u64)stat_list->next);
    while( temp != head ){
        chst = list_entry(temp, struct checkpoint_stats, list);
        printk("migration:%u, stack:%lu, heap:%u \n", chst->num, chst->stack_migrated_pages, chst->heap_migrated_pages);
        temp = temp->next;
    }
    return 0;
}

