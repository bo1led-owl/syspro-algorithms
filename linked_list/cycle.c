#include <stddef.h>

// https://leetcode.com/problems/linked-list-cycle-ii/submissions/1553656186/

struct ListNode {
    int val;
    struct ListNode* next;
};

struct ListNode* detectCycle(struct ListNode* head) {
    if (head == NULL) return NULL;

    // Brent's algorithm

    int power = 1;
    int lambda = 1;
    struct ListNode* tortoise = head;
    struct ListNode* hare = head->next;
    while (tortoise != hare) {
        if (tortoise == NULL || hare == NULL) return NULL;

        if (power == lambda) {
            tortoise = hare;
            power *= 2;
            lambda = 0;
        }
        hare = hare->next;
        lambda += 1;
    }

    tortoise = hare = head;
    while (lambda--) {
        hare = hare->next;
    }

    struct ListNode* start = head;
    while (tortoise != hare) {
        tortoise = tortoise->next;
        hare = hare->next;
        start = start->next;
    }

    return start;
}
