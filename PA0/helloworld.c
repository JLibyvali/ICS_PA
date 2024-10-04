#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define macro_print(x)               \
    {                                \
        printf("XXXXXXXXXXXXXXX\n"); \
        printf((x));                 \
        printf("\nXXXXXXXXXXXXXX\n");  \
    }

int main(int argc, char **argv)
{
    // Macro
    macro_print("Test macro printf")
    // Dynamic alloca array size.
    assert(argc > 1);
    int *arr = NULL;
    arr      = malloc(atoi(argv[1])*sizeof(int));

    assert(arr != NULL );
    arr[0] = 99;
    printf("Allocate: %d\n",arr[0]);
}