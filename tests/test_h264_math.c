#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h264decoder/h264_math.h"

int main(int argc, char **argv) {
    int result = 0;

    result = int_log2(100);
    printf("log2(100) = %d\n", result);

    result = int_log2(64);
    printf("log2(64) = %d\n", result);

    result = int_log2(0);
    printf("log2(0) = %d\n", result);

    result = int_log2(1);
    printf("log2(1) = %d\n", result);

    result = int_log2(2 * 100 + 1);
    printf("log2(2*100+1) = %d\n", result);

    result = int_ceil_log2(100);
    printf("int_ceil_log2(100) = %d\n", result);

    return EXIT_SUCCESS;
}