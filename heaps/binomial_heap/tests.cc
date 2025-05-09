#include <gtest/gtest.h>

#include "binomial_heap.hh"

namespace {
TEST(BinomialHeap, Basic) {
    BinomialHeap<int> a;

    constexpr int SIZE = 15;

    for (int i = 0; i < SIZE; ++i) {
        a.insert(i);
    }

    int check[SIZE];
    std::fill_n(check, SIZE, 0);

    for (auto x : a) {
        check[x] += 1;
    }

    for (int i = 0; i < SIZE; ++i) {
        EXPECT_EQ(check[i], 1);
    }
}

TEST(BinomialHeap, DecreaseKey) {
    BinomialHeap<int> a;

    constexpr int SIZE = 15;

    BinomialHeap<int>::iterator iters[SIZE];
    for (int i = 0; i < SIZE; ++i) {
        iters[i] = a.insert(i + 1);
    }

    int check[SIZE];
    std::fill_n(check, SIZE, 0);

    a.insert(iters[4], 4);

    for (auto x : a) {
        check[x - 1] += 1;
    }

    for (int i = 0; i < SIZE; ++i) {
        if (i == 3) {
            EXPECT_EQ(check[i], 2);
        } else if (i == 4) {
            EXPECT_EQ(check[i], 0);
        } else {
            EXPECT_EQ(check[i], 1);
        }
    }
}

TEST(BinomialHeap, ComplexType) {
    auto lt = [](const auto& a, const auto& b) { return *a < *b; };
    BinomialHeap<std::unique_ptr<int>, decltype(lt)> a{lt};

    constexpr int SIZE = 15;

    decltype(a)::iterator iters[SIZE];
    for (int i = 0; i < SIZE; ++i) {
        iters[i] = a.emplace(new int{42});
    }

    a.emplace_hint(iters[4], new int{4});
    a.emplace_hint(iters[6], new int{1});

    EXPECT_EQ(**a.top(), 1);

    a.erase(iters[6]);
    EXPECT_EQ(**a.top(), 4);
}

void foo(const BinomialHeap<int>& heap) {
    constexpr int SIZE = 15;
    int check[SIZE];
    std::fill_n(check, SIZE, 0);

    for (int x : heap) {
        check[x] += 1;
    }

    for (int i = 0; i < SIZE; ++i) {
        EXPECT_EQ(check[i], 1);
    }
}

TEST(BinomialHeap, ConstRef) {
    BinomialHeap<int> a;

    constexpr int SIZE = 15;

    for (int i = 0; i < SIZE; ++i) {
        a.insert(i);
    }

    foo(a);
}
}  // namespace
