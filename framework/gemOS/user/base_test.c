#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
     int a[2048];
     start_dtrack();
     a[0] = 1;
     a[1025] = 3;
     end_dtrack();
     sleep(1000);
     read_dbitmap();
     return 0;
}
