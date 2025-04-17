#include <stddef.h>
#include <stdio.h>

// https://leetcode.com/problems/reverse-linked-list-ii/submissions/1553643194/

struct ListNode {
    int val;
    struct ListNode* next;
};

static struct ListNode* reverseN(struct ListNode* head, int n) {
    struct ListNode* cur = head;
    struct ListNode* next = cur->next;

    for (int i = 1; i < n; ++i) {
        if (cur == head) {
            struct ListNode* tmp = cur->next;
            struct ListNode* nxt = next->next;
            cur->next = NULL;
            next->next = cur;
            cur = tmp;
            next = nxt;
        } else {
            struct ListNode* nxt = next->next;
            next->next = cur;
            cur = next;
            next = nxt;
        }
        if (i == n - 1) {
            head->next = next;
        }
    }

    return cur;
}

struct ListNode* reverseBetween(struct ListNode* head, int left, int right) {
    if (left == 1) {
        return reverseN(head, right - left + 1);
    }

    struct ListNode* revstart = head;
    for (int i = 1; i < left - 1; ++i) {
        revstart = revstart->next;
    }

    revstart->next = reverseN(revstart->next, right - left + 1);
    return head;
}

static void print(struct ListNode* head) {
    while (head != 0) {
        printf("%d -> ", head->val);
        head = head->next;
    }
    printf("null\n");
}

int main(void) {
    struct ListNode a = {.val = 1}, b = {.val = 2}, c = {.val = 3, .next = NULL};
    a.next = &b;
    b.next = &c;

    print(&a);
    print(reverseBetween(&a, 2, 3));
}
