#include<page.h>
#include<lib.h>
#include<mmap.h>
#include<memory.h>
#include<apic.h>
#include<context.h>


//Returns the pfn_info corresponding to the PFN passed as index argument.
struct pfn_info * get_pfn_info(u32 index){
    struct pfn_info * p = ((struct pfn_info *)list_pfn_info.start)+index;
    return p;
}

//Sets refcount of pfn_info corresponding to the PFN passed as index argument.
void set_pfn_info(u32 index){
    struct pfn_info * p = ((struct pfn_info *)list_pfn_info.start)+index;
    p->refcount = 1;
}
void reset_pfn_info(u32 index){
    struct pfn_info * p = ((struct pfn_info *)list_pfn_info.start)+index;
    p->refcount = 0;
}

//Increments refcount of pfn_info object passed as argument.
void increment_pfn_info_refcount(struct pfn_info * p){
    p->refcount+=1;
    return;
}

//Decrements refcount of pfn_info object passed as argument.
void decrement_pfn_info_refcount(struct pfn_info * p){
    p->refcount-=1;
    return;
}

//Get refcount of pfn_info object passed as argument.
u8 get_pfn_info_refcount(struct pfn_info *p){
    return p->refcount;
}
void init_pfn_info(u64 startpfn)
{
    list_pfn_info.start = (void *)(startpfn << PAGE_SHIFT);
    list_pfn_info.end = list_pfn_info.start + ((u64)NUM_PAGES << PAGE_SHIFT);
}

s8 get_pfn_refcount(u32 pfn){
    struct pfn_info *info = get_pfn_info(pfn);
    return info->refcount;
}
s8 get_pfn(u32 pfn)
{
    struct pfn_info *info = get_pfn_info(pfn);
    if(info->refcount < 1){
         printk("%s: Error in PFN refcounting count %d\n", __func__, info->refcount);
	 OS_BUG("PFN refcount");
    }
    info->refcount++;
    return info->refcount;   
}

s8 put_pfn(u32 pfn)
{
    struct pfn_info *info = get_pfn_info(pfn);
    if(info->refcount < 1){
         printk("%s: Error in PFN refcounting count %d\n", __func__, info->refcount);
	 OS_BUG("PFN refcount");
    }
    info->refcount--;  
    return info->refcount; 
}
/*Page table mapping APIs*/
/*Page table handling code for all types of page table manipulation*/
/*Definitions*/

static inline void invlpg(unsigned long addr) {
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}


static int purge_pte(u64 pte, u64 start, u64 end)
{
  int retval = 1, i;
  unsigned long *ptep = (unsigned long *)(pte << PAGE_SHIFT);
  unsigned start_entry = ((start & PTE_MASK) >> PTE_SHIFT);
  for(i=0; i < start_entry; ++i){
      if(*ptep)
           retval = 0;
      ptep++;
  }
  do{
      u64 pfn = (*ptep) >> PTE_SHIFT;
      if(pfn){
	       if(!put_pfn(pfn))
                    os_pfn_free(USER_REG, pfn);     //TODO: handle which region is freed. At this point, it is always the user region
               *ptep = 0;
               invlpg(start);
      }
      start += (0x1UL << PTE_SHIFT);
      start = (start >> PTE_SHIFT) << PTE_SHIFT;
      ptep++;
      ++i;
  }while(start < end && i != 512);

  for(; i < 512; ++i){
      if(*ptep)
           retval = 0;
      ptep++;
  }
  return retval;
}

static int purge_pmd(u64 pmd, u64 start, u64 end)
{
  int retval = 1, i;
  unsigned long *ptep = (unsigned long *)(pmd << PAGE_SHIFT);
  unsigned start_entry = ((start & PMD_MASK) >> PMD_SHIFT);
  for(i=0; i < start_entry; ++i){
      if(*ptep)
           retval = 0;
      ptep++;
  }
  do{
      u64 pfn = (*ptep) >> PTE_SHIFT;
      if(IS_PTE_HUGE(*ptep))
          *ptep = 0;
      else if(pfn){
           int purge = purge_pte(pfn, start, end);
           retval &= purge;
           if(purge){
               *ptep = 0;
	       if(!put_pfn(pfn))
                    os_pfn_free(OS_PT_REG, pfn);
           }
      }
      start += (0x1UL << PMD_SHIFT);
      start = (start >> PMD_SHIFT) << PMD_SHIFT;
      ptep++;
      ++i;
  }while(start < end && i != 512);

  for(; i < 512; ++i){
      if(*ptep)
           retval = 0;
      ptep++;
  }
  return retval;
}

static int purge_pud(u64 pud, u64 start, u64 end)
{
  int retval = 1, i;
  unsigned long *ptep = (unsigned long *)(pud << PAGE_SHIFT);
  unsigned start_entry = ((start & PUD_MASK) >> PUD_SHIFT);
  for(i=0; i < start_entry; ++i){
      if(*ptep)
           retval = 0;
      ptep++;
  }
  do{
      u64 pfn = (*ptep) >> PTE_SHIFT;
      if(pfn){
           int purge = purge_pmd(pfn, start, end);
           retval &= purge;
           if(purge){
               *ptep = 0;
	       if(!put_pfn(pfn))
                  os_pfn_free(OS_PT_REG, pfn);
           }
      }
      start += (0x1UL << PUD_SHIFT);
      start = (start >> PUD_SHIFT) << PUD_SHIFT;
      ptep++;
      ++i;
  }while(start < end && i != 512);

  for(; i < 512; ++i){
      if(*ptep)
           retval = 0;
      ptep++;
  }
  return retval;
}

int purge_mapping_range(struct exec_context *ctx, u64 start, u64 end)
{
  unsigned long *pgd = (unsigned long *)(((u64)ctx->pgd) << PAGE_SHIFT);
  unsigned long *ptep;
  if(end <= start){
      printk("Invalid range\n");
      return -1;
  }

  ptep = (unsigned long *)pgd + ((start & PGD_MASK) >> PGD_SHIFT);
  do{
      u64 pfn = (*ptep) >> PTE_SHIFT;
      if(pfn && purge_pud(pfn, start, end)){
           *ptep = 0;
	   if(!put_pfn(pfn))
              os_pfn_free(OS_PT_REG, pfn);
      }
      start += (0x1UL << PGD_SHIFT);
      start = (start >> PGD_SHIFT) << PGD_SHIFT;
      ptep++;
  }while(start < end);

  if(start == 0 && end >= STACK_START)
     return 1;   // Purge the pgd if you want
  return 0;
}

int destroy_user_mappings(struct exec_context *ctx)
{
   return purge_mapping_range(ctx, CODE_START, STACK_START);
}


