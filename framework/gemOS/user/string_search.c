#include<ulib.h>

unsigned int strlen_user(char* s) {
    int i=0; 
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

void search(char* pat, char* txt)
{
    int M = strlen_user(pat);
    int N = strlen_user(txt);

 
    /* A loop to slide pat[] one by one */
    for (int i = 0; i <= N - M; i++) {
        int j;
 
        /* For current index i, check for pattern match */
        for (j = 0; j < M; j++)
            if (txt[i + j] != pat[j])
                break;
 
        // if (j == M) // if pat[0...M-1] = txt[i, i+1, ...i+M-1]
        //     printf("Pattern found at index %d \n", i);
    }
}
 
/* Driver program to test above function */
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

    checkpoint_init(CP_INTERVAL);
    printf("String search\n");
    printf("CP_INTERVAL = %d\n", CP_INTERVAL);

    int times = 20000;

    char txt[] = "AABAACAADAABAAABAAAABAACAADAABAAABAA";
    char pat[] = "AABA";

    while (times--) {
        search(pat, txt);
    }
    return 0;
}