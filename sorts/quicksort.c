#include <stddef.h>
#include <stdlib.h>

// lomuto:
// hoare: https://leetcode.com/problems/sort-an-array/submissions/1545275975/

static void swap(int* a, size_t i, size_t j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

static size_t lomutoPartition(int* a, size_t l, size_t r) {
    size_t pivot_index = l + rand() % (r - l + 1);
    int pivot = a[pivot_index];

    size_t i = l - 1;
    for (size_t j = l; j <= r; ++j) {
        if (a[j] <= pivot) {
            i += 1;
            if (i != j) swap(a, i, j);
        }
    }

    return i + 1;
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

    size_t pivot = lomutoPartition(a, l, r);

    quickSort(a, l, pivot - 1);
    quickSort(a, pivot, r);
}

int* sortArray(int* nums, int numsSize, int* returnSize) {
    quickSort(nums, 0, numsSize - 1);
    *returnSize = numsSize;
    return nums;
}
