#include<ulib.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   // unsigned idx = rand();
   // while (idx > 0) {
   //    printk()
   // }
    checkpoint_init(CP_INTERVAL);
    printf("Strided\n");
    printf("CP_INTERVAL = %d\n", CP_INTERVAL);

   int size = (1 << 16);
   volatile char arr[size];


   int times = 60;


    while (times--) {
        for (int i=0; i<size; i+=64) {
            arr[i] = 'A';

            volatile int j = 100;
            while (j) {j--;}
        }
    }

   return 0;
}