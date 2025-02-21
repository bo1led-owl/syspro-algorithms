#include <stddef.h>
#include <stdlib.h>

// lomuto: https://leetcode.com/problems/sort-an-array/submissions/1550672550/
// hoare: https://leetcode.com/problems/sort-an-array/submissions/1545275975/

#define LOMUTO

static void swap(int* a, size_t i, size_t j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

typedef struct {
    size_t l, h;
} PartitionResult;

static PartitionResult lomutoPartition(int* a, size_t low, size_t high) {
    size_t pivot_index = low + rand() % (high - low + 1);
    int pivot = a[pivot_index];

    size_t l, h;
    l = h = low;

    for (size_t c = low; c <= high; ++c) {
        if (a[c] < pivot) {
            int tmp = a[c];
            a[c] = a[h];
            a[h] = a[l];
            a[l] = tmp;
            ++l;
            ++h;
        } else if (a[c] == pivot) {
            swap(a, h, c);
            ++h;
        }
    }

    return (PartitionResult){l, h};
}

static size_t hoarePartition(int* a, size_t l, size_t r) {
    size_t pivot_index = l + rand() % (r - l + 1);
    int pivot = a[pivot_index];

    size_t i = l - 1;
    size_t j = r + 1;

    for (;;) {
        do {
            ++i;
        } while (a[i] < pivot);

        do {
            --j;
        } while (a[j] > pivot);

        if (i >= j) {
            return j;
        }

        swap(a, i, j);
    }
}

static void quickSort(int* a, size_t l, size_t r) {
    if (l >= r) {
        return;
    }

#if defined(HOARE)
    size_t pivot = hoarePartition(a, l, r);

    quickSort(a, l, pivot);
    quickSort(a, pivot + 1, r);
#elif defined(LOMUTO)
    PartitionResult pivots = lomutoPartition(a, l, r);

    if (pivots.l > 0) quickSort(a, l, pivots.l - 1);
    quickSort(a, pivots.h, r);
#else
#error No partition algorithm chosen
#endif
}

int* sortArray(int* nums, int numsSize, int* returnSize) {
    quickSort(nums, 0, numsSize - 1);
    *returnSize = numsSize;
    return nums;
}
