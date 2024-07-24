#include<ulib.h>

/* Micro benchmark #1
   Pages dirtied = 3
   Average bytes modified = 1064
*/

void worker() {
    char arr[4096 * 2];
    for (int i=0; i<(4096 * 2); i+=8) {
        arr[i] = 'A';
    }
    // checkpoint_now();
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   checkpoint_init();

   for (int j=0; j<10; j++) {
      worker();
      worker();
      checkpoint_now();
   }

   return 0;
}
