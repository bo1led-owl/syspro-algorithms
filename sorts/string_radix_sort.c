#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static void swapPtrs(void** a, void** b) {
    void* tmp = *a;
    *a = *b;
    *b = tmp;
}

static void radixSortReorder(char** dest, char** src, size_t n, size_t* counts, size_t char_index) {
    for (size_t i = 0; i < 255; ++i) {
        counts[i + 1] += counts[i];
    }

    for (size_t i = n; i-- > 0;) {
        unsigned char ch = src[i][char_index];
        dest[counts[ch] - 1] = src[i];
        counts[ch] -= 1;
    }
}

static void sortStrings(char** strs, char** scratch, size_t n, size_t elem_len) {
    size_t counts[256];

    for (size_t i = elem_len; i-- > 0;) {
        bzero(counts, sizeof(counts));
        for (size_t j = 0; j < n; ++j) {
            unsigned char ch = strs[j][i];
            counts[ch] += 1;
        }

        radixSortReorder(scratch, strs, n, counts, i);
        swapPtrs((void**)strs, (void**)scratch);
    }

    if (elem_len % 2 != 0) memcpy(strs, scratch, sizeof(char*) * n);
}

#define SZ 13

static int strCompare(const void* lhs_void, const void* rhs_void) {
    char* const* const lhs = lhs_void;
    char* const* const rhs = rhs_void;
    return strcmp(*lhs, *rhs);
}

int main(void) {
    char* strs[SZ] = {"hello", "world", "aboba", "wheel", "robot", "debug", "wrong",
                      "clown", "crane", "build", "stack", "folds", "house"};

    char* correct[SZ];
    memcpy(correct, strs, sizeof(strs));
    qsort(correct, SZ, sizeof(char*), strCompare);

    char* buf[SZ];
    memcpy(buf, strs, sizeof(strs));
    sortStrings(strs, buf, SZ, 5);

    bool res = true;
    for (size_t i = 0; i < SZ; ++i) {
        if (strcmp(strs[i], buf[i]) != 0) {
            printf("MISMATCH: \"%s\" != \"%s\"\n", strs[i], buf[i]);
            res = false;
        }
    }

    if (res) {
        printf("OK\n");
    }
}
