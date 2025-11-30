#include "set.hh"

#include <cstdlib>
#include <utility>

namespace {
template <typename T> [[nodiscard]] T* checkedMalloc(size_t n) {
    T* res = std::bit_cast<T*>(malloc(n * sizeof(T)));
    if (!res) {
        throw std::bad_alloc();
    }
    return res;
}

template <typename T> [[nodiscard]] T* checkedCalloc(size_t n) {
    T* res = std::bit_cast<T*>(calloc(n, sizeof(T)));
    if (!res) {
        throw std::bad_alloc();
    }
    return res;
}

template <typename T> T* checkedRealloc(T*& p, size_t n) {
    T* res = std::bit_cast<T*>(realloc(p, n * sizeof(T)));
    if (!res) {
        throw std::bad_alloc();
    }
    return p = res;
}

constexpr size_t MASK_BITS = sizeof(uint64_t) * 8;

size_t maskIndex(size_t bitIndex) noexcept { return bitIndex / MASK_BITS; }
} // namespace

Set::Set(std::initializer_list<size_t> l) : Set() {
    for (size_t i : l) {
        insert(i);
    }
}

Set::Set(const Set& that)
    : repr_{checkedMalloc<uint64_t>(that.masks_)}, masks_{that.masks_}, size_{that.size_} {
    std::copy(that.repr_, that.repr_ + that.masks_, repr_);
}

Set& Set::operator=(const Set& that) {
    if (&that == this) {
        return *this;
    }

    Set newSet(that);
    *this = std::move(newSet);
    return *this;
}

Set::Set(Set&& that)
    : repr_{std::exchange(that.repr_, nullptr)}, masks_{std::exchange(that.masks_, 0)},
      size_{std::exchange(that.size_, 0)} {}

Set& Set::operator=(Set&& that) {
    std::swap(repr_, that.repr_);
    std::swap(masks_, that.masks_);
    return *this;
}

size_t Set::bits() const noexcept { return masks_ * MASK_BITS; }

size_t Set::size() const noexcept { return size_; }

bool Set::contains(size_t item) const noexcept {
    if (maskIndex(item) >= masks_) {
        return false;
    }

    uint64_t mask = repr_[maskIndex(item)];
    return (mask >> (item % MASK_BITS)) & 1;
}

void Set::insert(size_t item) noexcept {
    if (maskIndex(item) >= masks_) {
        checkedRealloc<uint64_t>(repr_, maskIndex(item) + 1);
        size_t prevMasks = masks_;
        masks_ = maskIndex(item) + 1;
        std::fill(repr_ + prevMasks, repr_ + masks_, 0);
    }

    uint64_t prevMask = repr_[maskIndex(item)];
    repr_[maskIndex(item)] = prevMask | (1z << (item % MASK_BITS));

    if (prevMask != repr_[maskIndex(item)]) {
        size_ += 1;
    }
}

void Set::erase(size_t item) noexcept {
    if (maskIndex(item) >= masks_) {
        return;
    }

    uint64_t prevMask = repr_[maskIndex(item)];
    repr_[maskIndex(item)] = prevMask & ~(1z << (item % MASK_BITS));

    if (prevMask != repr_[maskIndex(item)]) {
        size_ -= 1;
    }
}

bool Set::operator==(const Set& that) const noexcept {
    return size() == that.size() &&
           std::equal(repr_, repr_ + std::min(masks_, that.masks_), that.repr_);
}

bool Set::operator!=(const Set& that) const noexcept { return !(operator==(that)); }

Set::~Set() { free(repr_); }

void Set::Iterator::findNextSet() {
    if (!b_) {
        return;
    }

    while (i_ < b_->bits() && !b_->contains(i_)) {
        i_++;
    }
    i_++;
}

bool Set::Iterator::isEnd() const noexcept { return b_ == nullptr || i_ >= b_->bits(); }

Set::Iterator::Iterator(const Set* b) : b_{b} { findNextSet(); }

Set::Iterator& Set::Iterator::operator=(const Iterator& that) {
    b_ = that.b_;
    i_ = that.i_;
    return *this;
}

size_t Set::Iterator::operator*() const noexcept { return i_ - 1; }

Set::Iterator& Set::Iterator::operator++() {
    findNextSet();
    return *this;
}

Set::Iterator Set::Iterator::operator++(int) {
    Iterator it(*this);
    operator++();
    return it;
}

bool Set::Iterator::operator==(const Iterator& that) const {
    if (isEnd() && that.isEnd()) {
        return true;
    }
    return b_ == that.b_ && i_ == that.i_;
}

namespace std {
size_t hash<Set>::operator()(const Set& b) const noexcept {
    size_t res = 0;
    for (size_t i = 0; i < b.masks_; ++i) {
        res ^= b.repr_[i];
    }
    return res;
};
} // namespace std
