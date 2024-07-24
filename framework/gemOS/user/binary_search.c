#include<ulib.h>

// A recursive binary search function. It returns
// location of x in given array arr[l..r] is present,
// otherwise -1
int binarySearch(int arr[], int l, int r, int x)
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
  
        // If the element is present at the middle
        // itself
        if (arr[mid] == x)
            return mid;
  
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);
  
        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
  
    // We reach here when element is not
    // present in array
    return -1;
}
  
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    // int arr[] = { 2, 3, 4, 10, 40 };
    checkpoint_init(CP_INTERVAL);
    printf("Binary search\n");
    printf("CP_INTERVAL = %d\n", CP_INTERVAL);

    int size = 2000;
    int arr[size];

    for (int i=0; i<size; i++) {
        arr[i] = i;
    }


    int times = 50;
    unsigned seed = 0xACE1u;
    unsigned bit = 0;


    while (times--) {


        for (int i=0; i<size; i++) {
            binarySearch(arr, 0, size - 1, rand(&seed, &bit) % size);
        }

        // printf("times = %d\n", times);
    }

    return 0;
}