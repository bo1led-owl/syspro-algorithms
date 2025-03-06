#include <assert.h>
#include <stddef.h>
#include <stdio.h>

// https://leetcode.com/problems/sort-an-array/submissions/1545015646/

typedef struct {
    int* begin;
    int* end;
} Slice;

#define make(b, e) ((Slice){.begin = (b), .end = (e)})

#define len(s) ((s).end - (s).begin)

#define print(s)                                                           \
    {                                                                      \
        for (int* _p = (s).begin; _p != (s).end; ++_p) printf("%d ", *_p); \
        putchar('\n');                                                     \
    }

static void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static void merge(const Slice l, const Slice r, int* buf) {
    int* li = l.begin;
    int* ri = r.begin;

    while (li < l.end && ri < r.end) {
        swap(buf++, (*li < *ri) ? li++ : ri++);
    }
    while (li < l.end) {
        swap(li++, buf++);
    }
    while (ri < r.end) {
        swap(ri++, buf++);
    }
}

static void inPlaceMergeSort(Slice a);

static void inPlaceMergeSortInternal(Slice a, int* buf) {
    if (len(a) == 0) {
        return;
    }
    if (len(a) == 1) {
        swap(buf, a.begin);
        return;
    }

    Slice lhs = make(a.begin, a.begin + len(a) / 2);
    Slice rhs = make(a.begin + len(a) / 2, a.end);

    inPlaceMergeSort(lhs);
    inPlaceMergeSort(rhs);

    merge(lhs, rhs, buf);
}

static void inPlaceMergeSort(Slice a) {
    if (len(a) <= 1) {
        return;
    }

    int* mid = a.begin + len(a) / 2;
    int* scratch = a.end - len(make(a.begin, mid));
    inPlaceMergeSortInternal(make(a.begin, mid), scratch);

    while (len(make(a.begin, scratch)) > 1) {
        mid = scratch;
        scratch = a.begin + (len(make(a.begin, mid)) + 1) / 2;

        inPlaceMergeSortInternal(make(scratch, mid), a.begin);
        merge(make(a.begin, a.begin + (mid - scratch)), make(mid, a.end), scratch);
    }

    if (len(make(a.begin, scratch)) == 1) {
        int* p = a.begin;
        while (p + 1 < a.end && *p > *(p + 1)) {
            swap(p, p + 1);
            ++p;
        }
    }
}

int* sortArray(int* nums, int numsSize, int* returnSize) {
    inPlaceMergeSort(make(nums, nums + numsSize));
    *returnSize = numsSize;
    return nums;
}

int main() {
    int a[] = {5, 1, 1, 2, 0, 0};
    Slice as = make(a, a + (sizeof(a) / sizeof(*a)));
    inPlaceMergeSort(as);
    print(as);
}
