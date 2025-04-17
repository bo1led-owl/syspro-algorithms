#include <stack>
#include <unordered_map>

// https://leetcode.com/problems/maximum-frequency-stack/submissions/1591828046/

class FreqStack {
    std::unordered_map<int, int> freq_;
    std::unordered_map<int, std::stack<int>> stacks_;
    int max_freq_ = 0;

public:
    FreqStack() = default;

    void push(int val) {
        freq_[val] += 1;
        if (freq_[val] > max_freq_) max_freq_ = freq_[val];
        stacks_[freq_[val]].push(val);
    }

    int pop() {
        int res = stacks_[max_freq_].top();
        freq_[res] -= 1;
        stacks_[max_freq_].pop();
        while (max_freq_ > 0 && stacks_[max_freq_].empty()) {
            max_freq_ -= 1;
        }
        return res;
    }
};
