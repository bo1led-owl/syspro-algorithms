#pragma once

#include <optional>
#include <ostream>

template <typename T> std::optional<T> optMin(std::optional<T> a, T b) {
    if (a.has_value()) {
        return std::min(*a, b);
    }
    return b;
}

template <typename T> std::ostream& operator<<(std::ostream& os, std::optional<T> opt) {
    if (opt.has_value()) {
        os << *opt;
    } else {
        os << "None";
    }
    return os;
}
