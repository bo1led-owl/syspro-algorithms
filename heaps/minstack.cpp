#include <stack>

// https://leetcode.com/problems/min-stack/submissions/1561625003/

class MinStack {
    std::stack<int> repr_;
    std::stack<int> mins_;

public:
    MinStack() = default;
    
    void push(int val) {
        repr_.push(val);
        int x = (mins_.empty() || val < mins_.top()) ? val : mins_.top();
        mins_.push(x);
    }

    void pop() {
        repr_.pop();
        mins_.pop();
    }

    int top() const {
        return repr_.top();
    }

    int getMin() const {
        return mins_.top();
    }
};
