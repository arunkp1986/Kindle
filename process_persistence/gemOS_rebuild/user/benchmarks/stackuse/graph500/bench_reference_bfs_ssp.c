#include<ulib.h>

#define NUM_ACCESS 200
#define NUM_ELEM 450
#define MICRO_SEC 3000 //3000 cycles
#define SIZE 1775

#define RDTSC_START()            \
        __asm__ volatile("RDTSCP\n\t" \
                         "mov %%edx, %0\n\t" \
                         "mov %%eax, %1\n\t" \
                         : "=r" (start_hi), "=r" (start_lo) \
                         :: "%rax", "%rbx", "%rcx", "%rdx");

#define RDTSC_STOP()              \
        __asm__ volatile("RDTSCP\n\t" \
                         "mov %%edx, %0\n\t" \
                         "mov %%eax, %1\n\t" \
                         "CPUID\n\t" \
                         : "=r" (end_hi), "=r" (end_lo) \
                         :: "%rax", "%rbx", "%rcx", "%rdx");


u64 uelapsed(u32 start_hi, u32 start_lo, u32 end_hi, u32 end_lo)
{
        u64 start = (((u64)start_hi) << 32) | start_lo;
        u64 end   = (((u64)end_hi)   << 32) | end_lo;
        return end-start;
}
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    u32 start_hi = 0, start_lo = 0, end_hi = 0, end_lo = 0;
    u32 target_count = MICRO_SEC*100; //values at 100 usec interval
    u32 loop_counter = 1024;
    u64 rdtsc_count = 0;
    register int count = 0;
    do{
        count = 0;
	rdtsc_count = 0;
	loop_counter = (loop_counter<<1);
        RDTSC_START();    
        while(count < loop_counter){
            count += 1;
	} 
        RDTSC_STOP();
        rdtsc_count = uelapsed(start_hi,start_lo,end_hi,end_lo);
    }while(rdtsc_count<target_count);
    printf("counter:%u rdtsc:%u\n",loop_counter,rdtsc_count);
    u16* input = (u16*)mmap(NULL, SIZE*sizeof(u16), PROT_READ | PROT_WRITE, 0);
    u32 index[NUM_ELEM] ={0,7,9,20,32,39,41,42,51,53,57,69,73,75,78,82,86,87,101,102,110,112,118,120,123,134,137,142,143,152,155,161,162,168,172,177,181,188,189,190,191,196,200,204,205,208,209,214,221,222,225,228,251,253,254,261,262,263,264,270,272,275,276,277,280,283,286,290,292,297,301,303,308,309,317,322,323,326,328,331,332,335,339,343,344,347,352,354,365,366,395,397,412,414,429,430,440,447,449,452,471,472,496,507,510,513,514,520,531,534,535,540,541,546,547,548,559,567,572,573,574,578,579,594,598,627,633,641,643,644,646,647,649,650,655,656,659,662,664,669,677,684,685,686,689,690,691,692,696,697,698,699,703,704,705,706,709,710,712,714,715,716,721,725,727,730,732,733,744,747,752,759,763,764,780,781,782,787,791,808,809,811,812,820,826,827,831,832,833,837,838,841,842,845,846,849,855,860,861,863,864,873,874,881,886,899,902,905,908,915,919,933,949,954,955,958,961,965,966,972,975,978,989,993,994,998,1002,1005,1011,1012,1018,1019,1020,1021,1022,1023,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071,1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1088,1089,1090,1091,1092,1093,1097,1105,1110,1118,1121,1123,1126,1127,1129,1133,1137,1138,1139,1141,1143,1146,1149,1156,1157,1182,1184,1186,1189,1192,1196,1199,1200,1206,1212,1216,1235,1241,1243,1248,1249,1257,1263,1264,1265,1271,1272,1273,1275,1281,1282,1296,1297,1307,1329,1330,1344,1352,1362,1364,1370,1374,1376,1539,1540,1541,1542,1545,1546,1552,1556,1557,1558,1559,1563,1564,1565,1566,1573,1575,1578,1582,1603,1606,1619,1623,1624,1627,1628,1632,1635,1637,1643,1644,1651,1652,1655,1660,1664,1667,1668,1669,1675,1680,1681,1682,1683,1685,1686,1688,1689,1694,1695,1701,1702,1710,1711,1714,1715,1716,1717,1723,1724,1725,1726,1727,1734,1740,1741,1749,1750,1751,1753,1754,1755,1756,1757,1758,1759,1760,1761,1762,1763,1764,1765,1766,1767,1768,1769,1770,1771,1772,1773,1774};
    u32 value[NUM_ELEM] ={8,48,8,8,8,8,40,24,24,16,24,16,24,8,24,40,12,8,8,8,8,8,8,8,40,4,32,24,28,8,8,8,92,8,8,4,8,48,80,16,8,40,8,16,8,8,40,24,48,48,8,4,4,9,16,8,96,4,2,16,8,48,104,88,4,4,8,4,24,16,56,8,4,8,16,24,68,12,32,16,8,16,16,24,8,12,20,8,48,8,8,32,6,4,8,12,48,36,8,48,48,77,36,72,24,16,16,8,8,48,40,24,64,4,84,37,8,8,48,24,8,24,24,8,40,8,8,8,8,16,32,32,24,40,8,16,8,8,8,32,32,16,80,88,88,64,32,45,8,96,88,37,44,96,85,8,12,16,4,56,88,69,20,8,4,4,8,12,8,16,48,8,40,8,8,88,32,8,64,24,40,8,56,64,48,16,48,88,77,16,32,32,24,8,48,12,4,4,8,16,8,40,8,8,24,32,8,40,8,24,40,4,8,40,8,8,16,32,16,4,4,8,4,4,4,8,4,12,8,48,32,104,56,88,80,52,8,97,74,112,112,60,76,68,84,48,40,24,72,40,10,36,36,48,92,64,24,72,96,84,33,8,80,104,16,56,40,64,40,40,104,112,72,88,69,24,104,77,24,76,80,77,12,92,88,53,20,36,8,96,24,8,88,32,16,56,64,56,8,64,40,24,4,8,40,8,24,8,16,32,8,8,16,40,8,8,64,64,64,16,48,8,16,8,8,8,8,24,8,8,8,48,64,64,8,8,16,4,8,40,56,44,20,4,8,40,16,8,8,16,24,45,8,8,8,8,48,8,16,8,104,68,36,64,24,16,96,68,68,80,56,100,60,89,32,8,24,8,8,24,8,48,36,8,32,48,48,8,8,48,8,8,8,32,8,24,64,40,8,32,8,8,40,16,40,40,8,80,40,32,16,32,16,8,64,56,16,48,56,48,56,32,8,32,8,104,72,48,40,56,56,48,48,64,56,48,48,24,40,120,64,40,8,56,56,56,56,56,56,40};
    unsigned prev = 0;
    for( int i=0; i<NUM_ELEM; i++ ){
        unsigned ind = index[i];
	unsigned val = value[i];
	while( prev < ind ){
	    input[prev] = 0;
	    prev += 1;
	}
	input[prev] = val;
	prev += 1;
    }
    unsigned size = 0;
    checkpoint_start();
    gem5_dump_stats();
    gem5_reset_stats();
    for( int j=0; j<NUM_ACCESS; j++ ){
        size = input[j%SIZE];
        if(size){
	    int array[size];
	    umemset(array,0,size);
	}
	count = 0;
	while(count < loop_counter){
            count += 1;
	}
    }
    checkpoint_end();
    gem5_dump_stats();
    gem5_reset_stats();
    checkpoint_stats();
    return 0;
}
