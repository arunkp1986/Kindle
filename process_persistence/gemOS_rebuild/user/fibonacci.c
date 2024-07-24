#include<ulib.h>


int fib(int n)
{
   if (n <= 1)
      return n;
   return fib(n-1) + fib(n-2);
}
 
  
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    checkpoint_init(CP_INTERVAL);
    printf("Fibonacci\n");
    printf("CP_INTERVAL = %d\n", CP_INTERVAL);

    // int size = 100;
    int times = 2;
    // unsigned seed = 0xACE1u;
    // unsigned bit = 0;


    while (times--) {
        int num = 28;
        printf("Fib of %d is %d\n", num, fib(num));
    }
    
    return 0;
}