#include<ulib.h>

#define NUM_ACCESS 2006
#define NUM_ELEM 559
#define MICRO_SEC 3000 //3000 cycles
#define SIZE 2006

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
    u32 index[NUM_ELEM] = {0,1,2,4,5,6,7,8,9,10,14,16,22,26,27,28,36,38,41,52,53,57,58,60,62,63,64,65,67,68,71,72,73,74,77,78,79,80,81,82,84,85,86,95,98,117,118,124,125,129,132,136,146,147,150,153,168,180,181,190,191,192,212,217,218,223,225,230,235,236,237,241,248,254,257,262,263,265,269,270,271,272,273,274,275,277,279,280,281,288,289,292,296,304,311,313,328,329,332,337,338,342,343,344,345,346,347,352,358,359,361,362,364,367,371,372,373,374,375,376,382,383,391,395,400,402,408,410,411,412,413,414,416,422,425,433,439,440,445,446,447,448,449,450,451,452,457,458,459,460,461,462,463,464,465,466,467,468,473,474,475,479,480,481,482,502,503,504,505,506,507,508,509,510,511,512,522,525,526,531,541,542,550,551,560,561,570,573,580,582,592,596,608,620,621,625,629,630,634,637,644,648,658,673,674,675,680,682,687,688,690,696,699,706,707,708,709,710,711,712,713,716,717,719,730,783,784,785,788,793,796,801,804,805,806,810,811,817,820,824,829,834,835,843,845,849,853,854,857,861,862,863,864,865,866,869,878,881,882,884,885,890,904,914,918,921,926,927,928,929,930,935,936,937,938,944,945,948,949,953,954,957,958,961,962,963,964,973,979,988,1006,1008,1010,1011,1012,1013,1019,1021,1023,1029,1030,1034,1035,1036,1037,1038,1039,1040,1041,1042,1044,1045,1046,1047,1049,1052,1056,1057,1058,1060,1061,1062,1064,1065,1070,1071,1073,1081,1082,1084,1089,1090,1091,1092,1093,1096,1097,1098,1101,1106,1108,1109,1126,1131,1144,1149,1153,1154,1155,1159,1160,1161,1162,1163,1164,1165,1166,1167,1168,1169,1170,1171,1172,1173,1180,1181,1185,1186,1187,1196,1199,1200,1203,1204,1216,1217,1219,1220,1221,1228,1229,1230,1239,1240,1242,1244,1245,1246,1263,1266,1271,1273,1274,1279,1280,1281,1286,1290,1295,1296,1297,1300,1310,1311,1314,1315,1316,1320,1322,1326,1328,1329,1336,1337,1338,1339,1340,1341,1347,1355,1370,1371,1373,1374,1375,1376,1377,1381,1382,1385,1386,1394,1424,1426,1427,1429,1430,1432,1433,1434,1435,1436,1439,1444,1448,1460,1461,1467,1470,1478,1479,1522,1526,1533,1545,1555,1563,1564,1569,1570,1571,1580,1586,1587,1592,1593,1595,1596,1597,1598,1599,1600,1601,1602,1603,1606,1608,1610,1611,1612,1613,1614,1615,1616,1617,1620,1624,1650,1662,1666,1670,1671,1672,1673,1674,1675,1676,1678,1679,1680,1681,1682,1683,1684,1685,1689,1690,1704,1713,1723,1737,1745,1746,1747,1757,1763,1766,1773,1774,1778,1779,1786,1787,1788,1792,1796,1802,1805,1826,1827,1860,1868,1869,1877,1880,1888,1889,1895,1899,1901,1904,1907,1922,1926,1930,1936,1938,1951,1955,1959,1960,1967,1968,1970,1988,1991,1997,2004,2005};
    u32 value[NUM_ELEM] = {16,16,16,4,12,20,20,28,4,4,8,8,8,8,40,40,4,8,8,8,8,16,8,56,24,32,44,16,12,8,8,32,12,16,16,40,20,16,40,13,5,20,8,13,8,8,8,16,40,1,24,8,68,24,12,8,16,16,8,20,5,8,8,24,64,16,8,64,48,16,8,32,8,8,8,48,36,21,32,64,40,24,72,56,16,1,12,72,5,32,60,84,36,8,65,8,4,8,24,24,8,64,53,92,16,56,68,32,24,12,16,16,8,4,32,8,40,60,12,4,57,20,8,16,8,8,8,28,48,20,48,68,8,56,8,8,24,32,16,20,32,20,8,5,40,40,32,40,8,40,64,64,64,16,64,45,64,1,8,24,32,61,70,1,8,32,81,52,92,8,64,60,56,52,40,30,8,16,16,40,1,8,64,12,16,80,32,8,8,24,72,8,1,8,8,24,8,32,8,24,52,8,24,16,76,24,8,24,8,24,8,8,24,32,40,32,40,56,24,17,8,32,28,1,8,8,8,16,1,8,16,8,24,84,24,68,40,8,16,32,8,56,32,9,16,17,16,8,1,10,96,56,64,28,4,8,8,41,44,8,8,24,8,9,8,16,64,72,32,56,64,48,56,72,8,24,8,16,16,64,8,40,32,72,40,56,56,8,48,8,20,8,48,48,72,8,8,8,8,48,16,20,40,36,8,16,48,16,28,8,48,8,24,1,24,8,24,92,24,32,60,16,24,8,8,8,8,40,24,24,32,9,56,56,32,8,1,16,8,8,8,32,8,40,8,41,64,1,8,16,36,44,5,8,73,52,92,16,24,80,52,68,16,32,1,8,1,34,41,8,8,24,24,32,64,40,36,40,20,48,60,64,48,64,16,24,84,8,8,16,8,8,8,80,24,48,1,24,16,76,24,16,16,16,32,76,8,9,16,9,64,40,1,66,64,44,68,16,32,8,40,8,8,48,40,32,72,8,68,16,8,8,8,16,24,24,16,32,56,32,48,8,12,8,32,40,40,16,8,24,16,16,16,16,16,8,48,8,40,16,56,8,24,4,24,64,40,24,48,20,24,24,40,16,8,16,8,8,4,16,64,32,48,16,9,2,16,1,5,8,9,48,24,40,20,8,32,16,21,40,57,32,24,48,8,36,8,1,8,56,8,24,48,48,40,8,16,49,8,24,8,48,41,16,16,5,5,16,32,40,8,52,32,16,8,64,28,9,32,8,40,8,8,40,16,48,8,8,24,24,8,48,8,8,8,40,24,72,29};
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