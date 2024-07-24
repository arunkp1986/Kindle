#ifndef __PAGE_H_
#define __PAGE_H
#include<types.h>



//Bit positions in X86 PTE


#define PF_ERR_P 0
#define PF_ERR_W 1
#define PF_ERR_USER 2
#define PF_ERR_RSVD 3
#define PF_ERR_FETCH 4
#define B_PTE_P 0
#define B_PTE_W 1
#define B_PTE_U 2
#define B_PTE_A 5
#define B_PTE_D 6
#define B_PMD_PS 7
#define B_PTE_G 8
#define B_PTE_XD 63


#define OS_RW_FLAGS ((1 << B_PTE_P) | (1 << B_PTE_W))
#define OS_RWG_FLAGS ((1 << B_PTE_P) | (1 << B_PTE_W) | (1 << B_PTE_G))
#define OS_RW_HP_FLAGS ((1 << B_PTE_P) | (1 << B_PTE_W) | (1 << B_PMD_PS))
#define OS_RWG_HP_FLAGS ((1 << B_PTE_P) | (1 << B_PTE_W) | (1 << B_PMD_PS) | (1 << B_PTE_G))

#define USER_RW_FLAGS ((1 << B_PTE_P) | (1 << B_PTE_W) | (1 << B_PTE_U))
#define USER_RO_FLAGS ((1 << B_PTE_P) | (1 << B_PTE_U))

#define IS_PTE_HUGE(p) (((p) >> B_PMD_PS) & 0x1)


struct pfn_info{
    u8 refcount;
};

struct pfn_info_list{
    void * start;
    void * end;
};

struct pfn_info_list list_pfn_info;

struct pfn_info * get_pfn_info(u32 index);

extern void set_pfn_info(u32 index);
extern void reset_pfn_info(u32 index);


void increment_pfn_info_refcount(struct pfn_info * p);

void decrement_pfn_info_refcount(struct pfn_info * p);

extern u8 get_pfn_info_refcount(struct pfn_info *p);

extern s8 get_pfn_refcount(u32 pfn);
extern s8 get_pfn(u32 pfn);
extern s8 put_pfn(u32 pfn);
#endif
