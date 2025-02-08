#include <array>
#include <cstddef>
#include <vector>

// https://leetcode.com/problems/h-index/submissions/1535618103/

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
        // increments by Sedgewick (1982)
        // we can limit them to 4193 because 1 <= N <= 5000 by the problem statement
        static std::array<size_t, 7> increments{4193, 1073, 281, 77, 23, 8, 1};

        for (size_t inc : increments) {
            insertionSortK(items, inc);
        }
    }

public:
    int hIndex(std::vector<int>& citations) {
        shellSort(citations);

        for (size_t i = 0; i < citations.size(); ++i) {
            int papers_left = citations.size() - i;
            if (citations[i] >= papers_left) {
                return papers_left;
            }
        }
        return 0;
    }
};
