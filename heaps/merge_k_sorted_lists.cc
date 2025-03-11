#include <cassert>
#include <queue>
#include <utility>

// https://leetcode.com/problems/merge-k-sorted-lists/submissions/1570215432/

struct ListNode {
    int val;
    ListNode* next;

    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

namespace {
std::pair<ListNode*, ListNode*> uncons(ListNode* head) {
    auto res = std::exchange(head->next, nullptr);
    return {head, res};
}
}  // namespace

class Solution {
public:
    ListNode* mergeKLists(std::vector<ListNode*>& lists) {
        auto cmp = [](const ListNode* lhs, const ListNode* rhs) { return lhs->val > rhs->val; };

        std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> q{cmp};
        for (auto& l : lists) {
            if (l) q.push(l);
        }

        ListNode* head = nullptr;
        ListNode* cur = nullptr;
        while (!q.empty()) {
            auto [n, h] = uncons(q.top());
            q.pop();

            if (h) q.push(h);

            if (!head) {
                head = cur = n;
            } else {
                cur->next = n;
                cur = n;
            }
        }

        if (cur) cur->next = nullptr;
        return head;
    }
};
