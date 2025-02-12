#include <array>
#include <vector>

// https://leetcode.com/problems/wiggle-sort-ii/submissions/1540534967/

class Solution {
    static void insertionSortK(std::vector<int>& items, size_t k) {
        for (size_t i = 1; i < items.size(); ++i) {
            for (size_t j = i; j >= k; j -= k) {
                size_t l = j - k;
                size_t r = j;

                if (items[l] <= items[r]) {
                    break;
                }
                std::swap(items[l], items[r]);
            }
        }
    }

    static void shellSort(std::vector<int>& items) {
        static std::array<size_t, 7> increments{4193, 1073, 281, 77, 23, 8, 1};

        for (size_t inc : increments) {
            insertionSortK(items, inc);
        }
    }

    static void quickSort(std::vector<int>& a) {
        quickSort(a, 0, a.size() - 1);
    }

    static void quickSort(std::vector<int>& a, size_t l, size_t r) {
        if (l >= r) {
            return;
        }

        size_t pivot = partition(a, l, r);

        quickSort(a, l, pivot);
        quickSort(a, pivot + 1, r);
    }

    static size_t partition(std::vector<int>& a, size_t l, size_t r) {
        int pivot = a[l];

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

            std::swap(a[i], a[j]);
        }
    }

public:
    void wiggleSort(std::vector<int>& nums) {
        std::vector<int> copy{nums};
        quickSort(copy);

        size_t m = (nums.size() - 1) / 2;
        size_t r = nums.size() - 1;
        for (size_t i = 0; i < nums.size(); ++i) {
            if (i % 2 == 0) {
                nums[i] = copy[m];
                --m;
            } else {
                nums[i] = copy[r];
                --r;
            }
        }
    }
};
