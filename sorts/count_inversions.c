#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// https://leetcode.com/problems/global-and-local-inversions/submissions/1545037735/

typedef struct {
    int* begin;
    int* end;
} Slice;

#define make(b, e) ((Slice){.begin = (b), .end = (e)})

#define len(s) ((s).end - (s).begin)

static size_t mergeAndCountSplitInv(Slice a, Slice b, int* buf) {
    size_t res = 0;

    int* pa = a.begin;
    int* pb = b.begin;
    while (pa != a.end && pb != b.end) {
        if (*pa <= *pb) {
            *buf++ = *pa++;
        } else {
            *buf++ = *pb++;
            res += len(make(pa, a.end));
        }
    }
    while (pa != a.end) {
        *buf++ = *pa++;
    }
    while (pb != b.end) {
        *buf++ = *pb++;
    }

    return res;
}

static size_t countGlobalInversions(Slice a) {
    if (len(a) <= 1) return 0;

    int* mid = a.begin + len(a) / 2;
    Slice left = make(a.begin, mid);
    Slice right = make(mid, a.end);

    size_t left_inv = countGlobalInversions(left);
    size_t right_inv = countGlobalInversions(right);

    int* buf = malloc(len(a) * sizeof(int));
    size_t split_inv = mergeAndCountSplitInv(left, right, buf);
    memcpy(a.begin, buf, len(a) * sizeof(int));
    free(buf);

    return left_inv + right_inv + split_inv;
}

static size_t countLocalInversions(Slice a) {
    size_t res = 0;
    for (int* p = a.begin; p + 1 != a.end; ++p) {
        res += (*p > *(p + 1));
    }

    return res;
}

bool isIdealPermutation(int* nums, int numsSize) {
    Slice s = make(nums, nums + numsSize);
    return countLocalInversions(s) == countGlobalInversions(s);
}
