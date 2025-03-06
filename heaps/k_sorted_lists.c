#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// https://leetcode.com/problems/merge-k-sorted-lists/submissions/1561613998/

struct ListNode {
    int val;
    struct ListNode* next;
};

typedef struct ListNode ListNode;

typedef struct {
    ListNode** items;
    size_t len;
    size_t cap;
} PriorityQueue;

static void swap(ListNode** a, size_t i, size_t j) {
    struct ListNode* tmp = a[i - 1];
    a[i - 1] = a[j - 1];
    a[j - 1] = tmp;
}

static int get(const PriorityQueue* pq, size_t i) {
    return pq->items[i - 1]->val;
}

static void push(PriorityQueue* pq, ListNode* x) {
    assert(pq->len <= pq->cap);

    if (pq->len == pq->cap) {
        pq->cap = (pq->cap < 8) ? 8 : pq->cap * 2;
        pq->items = realloc(pq->items, pq->cap * sizeof(ListNode*));
    }

    pq->items[pq->len++] = x;
    size_t cur = pq->len;
    size_t parent = cur / 2;
    while (parent >= 1 && get(pq, cur) < get(pq, parent)) {
        swap(pq->items, cur, parent);
        cur = parent;
        parent = cur / 2;
    }
}

static ListNode* pop(PriorityQueue* pq) {
    if (pq->len == 0) {
        return NULL;
    }

    ListNode* res = pq->items[0];

    swap(pq->items, 1, pq->len);

    size_t cur = 1;
    pq->len -= 1;
    while (cur * 2 <= pq->len) {
        size_t lchild = cur * 2;
        size_t rchild = cur * 2 + 1;

        size_t child;
        if (rchild <= pq->len) {
            int l = get(pq, lchild);
            int r = get(pq, rchild);

            if (get(pq, cur) < l && get(pq, cur) < r) {
                break;
            }
            child = (l <= r) ? lchild : rchild;
        } else {
            if (get(pq, cur) < get(pq, lchild)) {
                break;
            }
            child = lchild;
        }

        swap(pq->items, cur, child);
        cur = child;
    }

    return res;
}

ListNode* mergeKLists(ListNode** lists, size_t listsSize) {
    PriorityQueue pq = (PriorityQueue){0};

    for (size_t i = 0; i < listsSize; ++i) {
        ListNode* cur = lists[i];
        while (cur) {
            push(&pq, cur);
            cur = cur->next;
        }
    }

    if (pq.len == 0) {
        return NULL;
    }

    ListNode* head = pop(&pq);
    ListNode* cur = head;
    while (cur != NULL) {
        ListNode* next = pop(&pq);
        cur->next = next;
        cur = next;
    }

    free(pq.items);
    return head;
}

static ListNode* makeList(int* a, size_t n) {
    if (n == 0) {
        return NULL;
    }

    ListNode* head = malloc(sizeof(ListNode));
    head->val = *a;
    head->next = NULL;
    ListNode* cur = head;
    for (size_t i = 1; i < n; ++i) {
        cur->next = malloc(sizeof(ListNode));
        cur->next->val = a[i];
        cur->next->next = NULL;
        cur = cur->next;
    }

    return head;
}

static void freeList(ListNode* head) {
    ListNode* cur = head;
    while (cur) {
        ListNode* next = cur->next;
        free(cur);
        cur = next;
    }
}

static void printList(const ListNode* head) {
    const ListNode* cur = head;
    while (cur) {
        printf("%d -> ", cur->val);
        cur = cur->next;
    }
    printf("null\n");
}

int main(void) {
    int a[] = {1, 4, 5};
    int b[] = {1, 3, 4};
    int c[] = {2, 6};

    ListNode* lists[3];
    lists[0] = makeList(a, 3);
    lists[1] = makeList(b, 3);
    lists[2] = makeList(c, 2);

    printList(lists[0]);
    printList(lists[1]);
    printList(lists[2]);
    ListNode* l = mergeKLists(lists, 3);
    printList(l);

    freeList(l);
}
