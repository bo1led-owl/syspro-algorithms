#include <gtest/gtest.h>

#include "matrix.h"

namespace {
void fill(MatT* m, size_t n) {
    for (size_t i = 0; i < n * n; ++i) {
        m[i] = rand() % 256;
    }
}
}  // namespace

TEST(MatMul, BadMul) {
    srand(time(NULL));

    std::vector<MatT> expected, actual, lhs, rhs;

    for (size_t i = 3; i <= 10; ++i) {
        size_t n = ((size_t)1) << i;

        expected.resize(n * n);
        actual.resize(n * n);
        lhs.resize(n * n);
        rhs.resize(n * n);

        fill(lhs.data(), n);
        fill(rhs.data(), n);

        mulNaive(n, expected.data(), lhs.data(), rhs.data());
        mulBad(n, actual.data(), lhs.data(), rhs.data());

        EXPECT_EQ(expected, actual);
    }
}

TEST(MatMul, Strassen) {
    srand(time(NULL));

    std::vector<MatT> expected, actual, lhs, rhs;

    for (size_t i = 3; i <= 10; ++i) {
        size_t n = ((size_t)1) << i;

        expected.resize(n * n);
        actual.resize(n * n);
        lhs.resize(n * n);
        rhs.resize(n * n);

        fill(lhs.data(), n);
        fill(rhs.data(), n);

        mulNaive(n, expected.data(), lhs.data(), rhs.data());
        mulStrassen(n, actual.data(), lhs.data(), rhs.data());

        EXPECT_EQ(expected, actual);
    }
}
