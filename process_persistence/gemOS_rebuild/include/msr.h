#ifndef MSR_HEADER_F
#define MSR_HEADER_F

#define MISCREG_TRACK_START 0xD0010118
#define MISCREG_TRACK_END 0xD0010120
#define MISCREG_DIRTYMAP_ADDR 0xD0010128
#define MISCREG_LOG_TRACK_GRAN 0xD0010130

extern u64 readmsr(u32 msr);
extern void writemsr(u32 msr, u64 val);
#endif
