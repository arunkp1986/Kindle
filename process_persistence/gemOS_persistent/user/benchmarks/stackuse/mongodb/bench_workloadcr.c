#include<ulib.h>

#define NUM_ACCESS 1808
#define NUM_ELEM 569
#define MICRO_SEC 3000 //3000 cycles
#define SIZE 1808

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
    u32 index[NUM_ELEM] = {0,1,2,3,4,5,6,7,8,9,10,11,13,14,15,17,18,19,20,21,22,23,24,25,26,27,28,29,30,32,33,34,40,42,44,47,48,51,52,53,54,55,56,57,58,59,60,62,63,64,65,71,73,77,110,111,112,113,114,116,117,119,120,123,124,132,133,138,139,142,143,144,158,159,160,166,167,173,174,176,179,180,181,182,183,184,185,187,190,191,192,196,197,201,203,208,209,214,215,216,220,228,234,237,242,243,249,254,262,269,270,272,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,308,309,320,322,325,327,336,339,340,345,347,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,370,371,372,373,374,375,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,400,401,403,406,415,416,420,434,438,440,445,450,459,461,480,484,486,490,519,527,528,535,542,564,565,575,581,582,584,585,593,594,599,601,602,606,609,613,619,636,638,646,647,650,655,670,680,681,682,683,686,689,691,698,758,760,761,771,772,773,774,775,776,779,793,797,798,802,806,809,810,816,817,820,832,847,848,849,854,856,857,862,864,867,875,878,892,902,906,909,916,917,921,922,925,927,928,930,943,951,952,953,954,955,962,966,967,970,974,975,984,990,994,997,998,999,1000,1001,1002,1004,1006,1007,1008,1009,1013,1017,1020,1021,1030,1034,1041,1042,1045,1048,1049,1050,1051,1052,1053,1054,1055,1057,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1072,1073,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087,1088,1089,1092,1093,1094,1096,1098,1100,1101,1106,1110,1118,1119,1126,1137,1138,1139,1152,1155,1156,1157,1165,1173,1174,1175,1176,1177,1178,1179,1180,1181,1182,1183,1189,1190,1191,1192,1193,1194,1195,1196,1197,1198,1199,1200,1201,1202,1210,1224,1228,1229,1231,1232,1238,1239,1240,1241,1242,1243,1244,1245,1246,1247,1249,1250,1251,1252,1253,1254,1255,1257,1266,1270,1274,1282,1283,1290,1305,1312,1341,1344,1359,1367,1368,1369,1370,1373,1414,1416,1419,1420,1427,1428,1437,1438,1439,1440,1441,1445,1446,1447,1449,1450,1451,1452,1453,1454,1455,1456,1459,1460,1462,1463,1464,1465,1466,1467,1468,1469,1470,1471,1472,1473,1474,1475,1476,1477,1478,1479,1480,1482,1483,1484,1485,1487,1488,1489,1492,1493,1494,1496,1497,1506,1512,1520,1521,1522,1523,1524,1528,1531,1534,1539,1540,1543,1549,1550,1552,1558,1559,1565,1568,1569,1573,1574,1579,1584,1585,1586,1587,1591,1595,1603,1604,1605,1606,1626,1627,1628,1629,1630,1631,1632,1649,1650,1651,1652,1658,1662,1663,1666,1695,1704,1705,1708,1710,1711,1714,1715,1718,1720,1724,1725,1731,1736,1737,1742,1744,1757,1763,1764,1765,1770,1775,1778,1784,1788,1792,1796,1797,1801,1806,1807};
    u32 value[NUM_ELEM] = {32,16,24,24,20,8,20,20,24,16,4,4,12,8,20,16,24,16,24,24,8,8,8,16,24,12,16,24,16,32,40,40,40,4,8,24,36,48,36,16,32,20,24,12,36,20,20,40,4,20,16,28,8,13,8,24,8,24,28,8,8,25,8,16,13,8,8,32,24,16,72,24,8,89,40,65,12,5,8,13,4,5,8,8,64,20,8,8,8,8,8,56,32,16,8,16,48,48,16,8,32,8,8,8,56,8,5,8,8,60,60,8,49,69,72,56,76,21,48,40,40,40,37,64,65,13,8,8,8,8,8,8,8,8,16,16,40,8,16,8,65,80,40,17,32,24,56,56,64,24,64,37,88,39,8,56,24,4,48,40,8,28,8,16,16,45,88,49,44,48,28,32,24,28,36,8,42,32,20,8,24,8,24,8,64,8,8,1,8,24,8,8,24,8,40,8,32,8,48,8,1,8,56,28,8,48,28,8,32,24,48,8,8,8,48,8,24,8,8,8,8,12,16,28,32,8,28,8,8,8,16,8,8,8,16,16,64,32,64,72,8,8,1,16,8,16,40,8,8,16,16,36,8,24,8,84,24,8,8,16,32,8,24,72,8,8,9,8,16,40,16,56,8,8,16,20,8,8,8,88,56,24,8,8,8,32,16,8,8,8,56,16,48,24,64,72,8,8,8,24,64,72,16,1,1,16,24,8,16,32,8,32,49,41,64,32,40,16,16,16,8,40,24,72,40,24,40,32,8,8,1,40,8,8,28,56,56,40,40,52,24,24,48,64,48,24,8,8,8,16,1,8,8,16,24,12,8,4,32,24,56,36,8,8,8,56,52,8,8,60,32,8,72,52,64,24,56,64,8,48,56,48,64,24,64,64,32,8,17,16,24,48,56,56,8,24,17,16,24,56,57,72,32,56,48,8,8,12,52,8,72,56,40,32,49,8,25,8,32,56,48,8,8,48,8,8,32,1,16,48,88,88,40,24,8,24,8,20,98,24,56,56,48,32,32,36,8,4,8,28,8,64,16,48,32,24,48,8,80,16,64,72,8,8,8,72,64,64,16,24,32,48,40,24,56,24,24,40,32,24,48,8,8,4,32,32,8,12,8,36,8,80,64,16,9,16,32,24,8,24,20,32,24,8,24,40,8,8,48,4,4,8,8,48,4,1,8,32,36,1,40,48,56,88,24,40,48,33,8,40,72,28,1,8,16,8,16,8,21,4,12,24,8,72,44,16,16,8,32,8,32,20,4,8,4,72,88,104,8,32,8,8,48,24,40,16,16,8,40};
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
