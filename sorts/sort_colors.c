#include <stddef.h>

// https://leetcode.com/problems/sort-colors/submissions/1545265211/

static void swap(int* a, size_t i, size_t j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

void sortColors(int* nums, size_t numsSize) {
    size_t zeros = 0;
    size_t twos = numsSize - 1;

    size_t i = 0;
    while (i < twos) {
        if (nums[i] == 0 && i > zeros) {
            swap(nums, i, zeros++);
        } else if (nums[i] == 2 && i < twos) {
            swap(nums, i, twos--);
        } else {
            ++i;
        }
    }
}
