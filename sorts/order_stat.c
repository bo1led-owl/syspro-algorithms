#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void swap(int* a, size_t i, size_t j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

static size_t partition(int* a, size_t l, size_t r) {
    assert(l <= r);

    size_t pivot_index = l + rand() % (r - l + 1);
    int pivot = a[pivot_index];
    swap(a, pivot_index, r);

    size_t i = l;

    for (size_t j = l; j < r; ++j) {
        if (a[j] <= pivot) {
            swap(a, i, j);
            ++i;
        }
    }

    swap(a, i, r);
    return i;
}

static int kthOrderStat(int* data, size_t n, size_t k) {
    size_t l = 0;
    size_t r = n;

    for (;;) {
        size_t pivot = partition(data, l, r - 1);

        if (pivot == k) {
            return data[pivot];
        } else if (k < pivot) {
            r = pivot;
        } else {
            l = pivot + 1;
        }
    }
}

static int median(int* data, size_t n) {
    if (n % 2 != 0) return kthOrderStat(data, n, n / 2);
    else {
        int a = kthOrderStat(data, n, n / 2 - 1);
        int b = kthOrderStat(data, n, n / 2);
        return (a + b) / 2;
    }
}

static void check(int* data, size_t n) {
    int actual = median(data, n);
    long long actual_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        actual_sum += abs(data[i] - actual);
    }

    int min = data[0], max = data[0];
    for (size_t i = 0; i < n; ++i) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }

    long long bestsum = LLONG_MAX;
    for (int i = min; i <= max; ++i) {
        long long sum = 0;
        for (size_t j = 0; j < n; ++j) {
            sum += abs(data[j] - i);
        }

        if (sum < bestsum) {
            bestsum = sum;
        }
    }

    if (bestsum == actual_sum) {
        puts("OK");
    } else {
        printf("ERROR: expected sum %lld, got %lld\n", bestsum, actual_sum);
    }
}

int main(void) {
    srand(time(NULL));

#define SZ 64
    int* data = malloc(sizeof(int) * SZ);
    for (size_t i = 0; i < 32; ++i) {
        for (size_t j = 0; j < SZ; ++j) {
            data[j] = rand() % 256;
        }
        check(data, SZ);
    }
}
