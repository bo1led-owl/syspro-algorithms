#pragma once

#include <cassert>
#include <cstdint>
#include <functional>

class Set {
    friend struct std::hash<Set>;

    uint64_t* repr_;
    size_t masks_;
    size_t size_;

    size_t bits() const noexcept;

public:
    Set() : repr_{nullptr}, masks_{0}, size_{0} {}
    Set(std::initializer_list<size_t> l);

    Set(const Set& that);
    Set& operator=(const Set& that);
    Set(Set&& that);
    Set& operator=(Set&& that);

    size_t size() const noexcept;
    bool contains(size_t item) const noexcept;
    void insert(size_t item) noexcept;
    void erase(size_t item) noexcept;

    bool operator==(const Set& that) const noexcept;
    bool operator!=(const Set& that) const noexcept;
    ~Set();

    class Iterator {
        friend Set;

        const Set* b_;
        size_t i_ = 0;

        void findNextSet();

        bool isEnd() const noexcept;

        explicit Iterator(const Set* b);

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = size_t;

        Iterator() : b_{nullptr} {}
        Iterator(const Iterator& that) noexcept : b_{that.b_}, i_{that.i_} {}

        Iterator& operator=(const Iterator& that);
        size_t operator*() const noexcept;
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& that) const;
    };

    static_assert(std::forward_iterator<Iterator>);

    Iterator begin() const { return Iterator(this); }
    Iterator end() const { return Iterator(); }
    Iterator cbegin() const { return Iterator(this); }
    Iterator cend() const { return Iterator(); }
};

namespace std {
template <> struct hash<Set> {
    size_t operator()(const Set& b) const noexcept;
};
} // namespace std
