#include "number.hh"

#include <algorithm>
#include <cstring>

Number& Number::operator=(this Number& self, Number::Digit value) {
    self.clear();
    if (value > 0) {
        self.push_back(value);
    }

    return self;
}

std::strong_ordering Number::View::operator<=>(this View lhs, View rhs) {
    if (lhs.size() < rhs.size()) {
        return std::strong_ordering::less;
    } else if (lhs.size() > rhs.size()) {
        return std::strong_ordering::greater;
    }

    return std::lexicographical_compare_three_way(lhs.crbegin(), lhs.crend(), rhs.crbegin(),
                                                  rhs.crend());
}

std::strong_ordering Number::View::cmpWithOffset(this View lhs, View rhs, size_t offset) {
    auto ord = std::strong_order(lhs, rhs);
    if (ord != std::strong_ordering::equivalent) {
        return ord;
    }

    View rest = lhs.subview(0, offset);
    bool lhs_has_nonzero_digits =
        std::any_of(rest.cbegin(), rest.cend(), [](const Digit x) { return x != 0; });
    if (lhs_has_nonzero_digits) {
        return std::strong_ordering::greater;
    } else {
        return std::strong_ordering::equivalent;
    }
}

bool Number::View::operator==(this View lhs, View rhs) noexcept {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

bool Number::operator==(this const Number& lhs, const Number& rhs) noexcept {
    return lhs.view() == rhs.view();
}

Number::View Number::View::subview(this View self, size_t from) noexcept {
    if (from >= self.m_size) {
        return View{nullptr, 0};
    }

    return View{self.m_data + from, self.m_size - from};
}

Number::View Number::View::subview(this View self, size_t from, size_t to) noexcept {
    if (from >= self.m_size) {
        return View{nullptr, 0};
    } else if (to >= self.m_size) {
        return self.subview(from);
    }

    return View{self.m_data + from, to - from};
}

Number::Digit Number::View::operator[](this View self, size_t i) noexcept {
    assert(i < self.m_size);
    return self.m_data[i];
}

size_t Number::View::size(this View self) noexcept {
    return self.m_size;
}

Number::Digit Number::View::getOr(this View self, size_t i, Number::Digit default_val) noexcept {
    if (i >= self.m_size) {
        return default_val;
    }
    return self[i];
}

Number::Digit Number::getOr(this const Number& self, size_t i, Number::Digit default_val) noexcept {
    return self.view().getOr(i, default_val);
}

// Number::View::iterator Number::View::begin(this View self) noexcept {
//     return self.m_data;
// }

// Number::View::iterator Number::View::end(this View self) noexcept {
//     return self.m_data + self.m_size;
// }

// Number::View::reverse_iterator Number::View::rbegin(this View self) noexcept {
//     return reverse_iterator{self.end()};
// }

// Number::View::reverse_iterator Number::View::rend(this View self) noexcept {
//     return reverse_iterator{self.begin()};
// }

Number::View::const_iterator Number::View::cbegin(this View self) noexcept {
    return self.m_data;
}

Number::View::const_iterator Number::View::cend(this View self) noexcept {
    return self.m_data + self.m_size;
}

Number::View::const_reverse_iterator Number::View::crbegin(this View self) noexcept {
    return const_reverse_iterator{self.cend()};
}

Number::View::const_reverse_iterator Number::View::crend(this View self) noexcept {
    return const_reverse_iterator{self.cbegin()};
}

Number::View Number::view(this const Number& self) noexcept {
    return View{self};
}

Number::View Number::view(this const Number& self, size_t from) noexcept {
    return self.view().subview(from);
}

Number::View Number::view(this const Number& self, size_t from, size_t to) noexcept {
    return self.view().subview(from, to);
}

Number Number::View::operator+(this View lhs, View rhs) {
    Number a{lhs};
    a += rhs;
    return a;
}

Number Number::operator+(this const Number& lhs, Digit rhs) {
    if (rhs == 0) {
        return lhs;
    }

    return lhs.view() + View{&rhs, 1};
}

Number& Number::operator+=(this Number& lhs, Digit rhs) {
    if (rhs == 0) {
        return lhs;
    }

    return lhs += View{&rhs, 1};
}

Number& Number::operator+=(this Number& lhs, View rhs) {
    return lhs.addWithOffset(rhs, 0);
}

Number& Number::operator+=(this Number& lhs, const Number& rhs) {
    lhs += rhs.view();
    return lhs;
}

Number& Number::addWithOffset(this Number& lhs, View rhs, size_t offset) {
    if (rhs.size() == 0) {
        return lhs;
    }

    Number::Digit carry = 0;
    size_t maxlen = std::max(lhs.size(), rhs.size() + offset);
    lhs.resize(maxlen, 0);

    for (size_t i = offset; i < maxlen; ++i) {
        Number::Digit res;
        carry = __builtin_add_overflow(lhs[i], carry, &res);
        carry += __builtin_add_overflow(res, rhs.getOr(i - offset, 0), &lhs[i]);
    }

    if (carry > 0) {
        lhs.push_back(carry);
    }

    while (lhs.size() > 0 && lhs.back() == 0) {
        lhs.pop_back();
    }

    return lhs;
}

Number& Number::operator-=(this Number& lhs, View rhs) {
    return lhs.subWithOffset(rhs, 0);
}

Number& Number::operator-=(this Number& lhs, Digit rhs) {
    return lhs -= View{&rhs, 1};
}

Number& Number::subWithOffset(this Number& lhs, View rhs, size_t offset) {
    if (lhs.size() < rhs.size() + offset ||
        lhs.view().cmpWithOffset(rhs, offset) == std::strong_ordering::less) {
        lhs.clear();
        return lhs;
    }

    bool carry = 0;
    for (size_t i = offset; i < rhs.size() + offset || carry; ++i) {
        DoubleDigit cur = carry + static_cast<DoubleDigit>(rhs.getOr(i - offset, 0));

        if (lhs[i] >= cur) {
            lhs[i] -= cur;
            carry = 0;
        } else {
            lhs[i] = static_cast<Digit>(BASE - cur + lhs[i]);
            carry = 1;
        }
    }

    while (lhs.size() > 0 && lhs.back() == 0) {
        lhs.pop_back();
    }

    return lhs;
}

Number::Digit Number::div10(this Number& self) {
    Number::Digit carry = 0;
    for (size_t i = 0; i < self.size(); ++i) {
        size_t index = (self.size() - i - 1);

        Number::DoubleDigit cur = static_cast<Number::DoubleDigit>(self[index]) +
                                  static_cast<Number::DoubleDigit>(carry) * BASE;
        self[index] = (cur / 10) & MASK;
        carry = (cur % 10) & MASK;
    }

    while (self.size() > 0 && self.back() == 0) {
        self.pop_back();
    }

    assert(carry < 10);
    return carry;
}

std::string Number::toString(this Number const& self) {
    Number num{self};
    return std::move(num).toString();
}

std::string Number::toString(this Number&& self) {
    if (self.size() == 0) {
        return "0";
    }

    std::string s;
    while (self.size() > 0) {
        s.push_back((char)self.div10() + '0');
    }
    std::reverse(s.begin(), s.end());
    return s;
}

[[nodiscard]] Number Number::mulNaive(this const Number& lhs, const Number& rhs) {
    return lhs.view().mulNaive(rhs);
}

Number Number::View::mulNaive(this View lhs, View rhs) {
    Number buf{};

    for (size_t i = 0; i < lhs.size(); ++i) {
        Number::Digit carry = 0;

        for (size_t j = 0; j < rhs.size() || carry != 0; ++j) {
            if (i + j >= buf.size()) {
                buf.push_back(0);
            }

            Number::DoubleDigit cur = static_cast<Number::DoubleDigit>(lhs[i]) *
                                      static_cast<Number::DoubleDigit>(rhs.getOr(j, 0));
            cur += buf[i + j];
            cur += carry;

            buf[i + j] = cur & MASK;
            carry = (cur >> DIGIT_BITS) & MASK;
        }
    }

    return buf;
}

Number Number::mulKaratsuba(this const Number& lhs, const Number& rhs) {
    return lhs.view().mulKaratsuba(View{rhs});
}

Number Number::operator/(this const Number& lhs, const Number& rhs) {
    Number res = lhs;
    res /= rhs.view();
    return res;
}

Number& Number::operator/=(this Number& lhs, const Number& rhs) {
    return lhs /= rhs.view();
}
