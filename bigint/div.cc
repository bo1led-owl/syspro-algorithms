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
        std::optional<View> piece = std::nullopt;

        for (size_t i = divisible.size() - 1; i-- > 0;) {
            View p = divisible.view(i);
            if (p >= divisor) {
                piece = p;
                break;
            }
        }

        if (!piece.has_value()) {
            break;
        }

        Number cur{1};
        Number product{divisor};
        while (product < *piece) {
            product += divisor;
            cur += 1;
        }

        if (product != *piece) {
            product -= divisor;
            cur -= 1;
        }

        divisible.subWithOffset(product, divisible.size() - piece->size());
        res.insert(res.end(), cur.cbegin(), cur.cend());
    }

    std::reverse(res.begin(), res.end());
    std::swap(divisible, res);
    return divisible;
}

#ifdef DIV
#include "testing.hh"

namespace {
TEST(Div) {
    Number a{123'456, 78};
    Number b{35'456, 85};

    Number c = a.mulKaratsuba(b);
    c /= a;
    EXPECT_EQ(c, b);

    EXPECT_EQ(b / b, Number{1});
}
}  // namespace

int main() {
    RUN_TEST(Div);
}
#endif
