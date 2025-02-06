#include <gtest/gtest.h>

#include <algorithm>
#include <optional>
#include <stdexcept>

#include "number.hh"

Number& Number::operator/=(this Number& divisible, View divisor) {
    if (divisor.size() == 0) {
        throw std::invalid_argument{"Division by zero"};
    }

    Number res{};

    while (divisible.size() > 0) {
        std::optional<size_t> offset = std::nullopt;

        for (size_t i = divisible.size(); i-- > 0;) {
            if (divisible.view(i) >= divisor) {
                offset = i;
                break;
            }
        }

        if (!offset.has_value()) {
            break;
        }

        Digit cur = 0;
        size_t zeros = 0;

        while (divisible.view(*offset) >= divisor) {
            zeros = divisible.subWithOffset(divisor, *offset);
            cur += 1;
        }

        res.push_back(cur);
        res.resize(res.size() + zeros, 0);
    }

    std::reverse(res.begin(), res.end());
    std::swap(divisible, res);
    return divisible;
}

TEST(Div, Div) {
    Number a{123'456, 78};
    Number b{35'456, 85};

    Number c = a.mulKaratsuba(b);
    c /= a;
    EXPECT_EQ(c, b);

    EXPECT_EQ(b / b, Number{1});

    Number n200{0, 0, 2};
    Number n100{0, 0, 1};
    EXPECT_EQ(n200 / Number{2}, n100);
}
