#include <gtest/gtest.h>

#include "number.hh"

namespace {
size_t roundUpToPowerOf2(size_t n) {
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n += 1;

    return n;
}
}  // namespace

Number Number::View::mulKaratsuba(this View lhs, View rhs) {
    if (lhs.size() <= 1 && rhs.size() <= 1) {
        DoubleDigit res =
            static_cast<DoubleDigit>(lhs.getOr(0, 0)) * static_cast<DoubleDigit>(rhs.getOr(0, 0));

        if (res >= BASE) {
            return Number{{static_cast<Digit>(res & MASK), static_cast<Digit>(res >> DIGIT_BITS)}};
        } else if (res > 0) {
            return Number{static_cast<Digit>(res)};
        } else {
            return Number{};
        }
    }

    size_t size = roundUpToPowerOf2(std::max(lhs.size(), rhs.size()));
    size_t mid = size / 2;

    /*
    lhs: a b
    rhs: c d
    */

    View a = lhs.subview(mid);
    View b = lhs.subview(0, mid);
    View c = rhs.subview(mid);
    View d = rhs.subview(0, mid);

    Number ac = a.mulKaratsuba(c);
    Number bd = b.mulKaratsuba(d);

    Number p = (a + b).mulKaratsuba(c + d);
    p -= ac;
    p -= bd;

    bd.addWithOffset(ac, size);
    bd.addWithOffset(p, mid);

    return bd;
}

TEST(Mul, Naive) {
    Number a{485'062'007};
    a = a.mulNaive(Number{132'375'408});

    EXPECT_EQ(a, Number({315'681'040, 14'950'121}));
}

Number fibonacci(size_t n) {
    if (n == 0) {
        return Number{0};
    } else if (n <= 2) {
        return Number{1};
    }

    Number a{1}, b{1};

    for (size_t i = 3; i <= n; ++i) {
        a += b;
        std::swap(a, b);
    }

    return b;
}

TEST(Mul, Karatsuba) {
    // these numbers definitely have different length
    Number a{fibonacci(10'000)};
    Number b{fibonacci(5'000)};

    EXPECT_EQ(a.mulNaive(b), a.mulKaratsuba(b));
}
