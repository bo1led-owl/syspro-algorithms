#pragma once

#include <cassert>
#include <functional>
#include <iterator>
#include <memory>

template <typename T, typename Cmp = std::less<T>>
    requires(std::invocable<Cmp, const T&, const T&>)
class BinomialHeap {
    struct Node final {
        std::shared_ptr<Node> leftmost_child = nullptr;
        std::shared_ptr<Node> parent = nullptr;
        std::shared_ptr<Node> neighbour = nullptr;
        unsigned deg = 0;
        T value;

        Node() = default;

        template <typename S>
            requires(std::constructible_from<T, S>)
        Node(S&& val) : value{std::forward<S>(val)} {}
    };

    class Iterator final {
        friend BinomialHeap;

        std::shared_ptr<Node> node;

        explicit Iterator(std::shared_ptr<Node> node) noexcept : node{node} {}

    public:
        using difference_type = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;

        Iterator() : node{nullptr} {}

        reference operator*() const noexcept {
            return node->value;
        };

        Iterator& operator++() noexcept {
            if (!node) {
                return *this;
            }

            if (node->leftmost_child) {
                node = node->leftmost_child;
            } else if (node->neighbour) {
                node = node->neighbour;
            } else {
                std::shared_ptr<Node> cur = node;
                for (;;) {
                    if (cur->neighbour) {
                        node = cur->neighbour;
                        break;
                    } else if (cur->parent) {
                        cur = cur->parent;
                    } else {
                        node = nullptr;
                        break;
                    }
                }
            }
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator save{*this};
            ++*this;
            return save;
        }

        bool operator==(this const Iterator& lhs, const Iterator& rhs) noexcept {
            return lhs.node == rhs.node;
        }

        bool operator!=(this const Iterator& lhs, const Iterator& rhs) noexcept {
            return lhs.node != rhs.node;
        }
    };

public:
    using reference = T&;
    using const_reference = const T&;
    using value_compare = Cmp;
    using value_type = T;
    using size_type = size_t;
    using iterator = Iterator;
    using const_iterator = Iterator;

private:
    std::shared_ptr<Node> merge(std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs) noexcept {
        assert(lhs->deg == rhs->deg);

        if (!comp_(lhs->value, rhs->value)) {
            std::swap(lhs, rhs);
        }

        rhs->neighbour = lhs->leftmost_child;
        lhs->leftmost_child = rhs;
        lhs->deg += 1;
        return lhs;
    }

    iterator merge(std::shared_ptr<Node> node) noexcept {
        if (!node) {
            return iterator{nullptr};
        }

        if (!root_) {
            root_ = node;
            return iterator{root_};
        }

        std::shared_ptr<Node> cur = root_;
        std::shared_ptr<Node> prev = nullptr;
        while (cur) {
            if (node->deg <= cur->deg) {
                if (prev) {
                    prev->neighbour = node;
                } else {
                    root_ = node;
                }
                node->neighbour = cur;
                return iterator{node};
            } else if (cur->deg == node->deg) {
                if (prev) {
                    prev->neighbour = cur->neighbour;
                }
                node = merge(cur, node);
            } else {
                prev = cur;
                cur = cur->neighbour;
            }
        }

        prev->neighbour = node;
        return iterator{node};
    }

    iterator sieveUp(const_iterator pos) noexcept {
        std::shared_ptr<Node> cur = pos.node;
        while (cur && cur->parent && comp_(cur->value, cur->parent->value)) {
            std::swap(cur->value, cur->parent->value);
            cur = cur->parent;
        }

        return iterator{cur};
    }

    std::shared_ptr<Node> extract(const_iterator pos) noexcept {
        assert(pos.node->parent == nullptr);

        std::shared_ptr<Node> res = pos.node->leftmost_child;

        if (root_ == pos.node) {
            root_ = root_->neighbour;
        } else {
            std::shared_ptr<Node> cur = root_;
            while (cur->neighbour != pos.node) {
                cur = cur->neighbour;
            }
            cur->neighbour = pos.node->neighbour;
        }

        pos.node->parent = nullptr;
        pos.node->neighbour = nullptr;
        pos.node->leftmost_child = nullptr;
        pos.node->~Node();
        return res;
    }

    std::shared_ptr<Node> root_ = nullptr;
    Cmp comp_{};

public:
    BinomialHeap() = default;
    BinomialHeap(const Cmp& comp) : comp_{comp} {}

    BinomialHeap(BinomialHeap&) = delete;
    BinomialHeap& operator=(BinomialHeap&) = delete;

    BinomialHeap(BinomialHeap&& other) noexcept {
        swap(other);
    }

    BinomialHeap& operator=(BinomialHeap&& other) noexcept {
        swap(other);
    }

    void swap(BinomialHeap& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(comp_, other.comp_);
    }

    iterator begin() noexcept {
        return iterator{root_};
    }
    const_iterator begin() const noexcept {
        return iterator{root_};
    }
    const_iterator cbegin() const noexcept {
        return const_iterator{root_};
    }
    iterator end() noexcept {
        return iterator{nullptr};
    }
    const_iterator end() const noexcept {
        return iterator{nullptr};
    }
    const_iterator cend() const noexcept {
        return const_iterator{nullptr};
    }

    const_iterator top() const noexcept {
        std::shared_ptr<Node> cur = root_;
        std::shared_ptr<Node> min = cur;
        while (cur) {
            if (comp_(cur->value, min->value)) {
                min = cur;
            }
            cur = cur->neighbour;
        }
        return iterator{min};
    }

    iterator insert(const T& value) {
        auto node = std::make_shared<Node>(value);
        return merge(node);
    }

    iterator insert(T&& value) {
        auto node = std::make_shared<Node>(std::move(value));
        return merge(node);
    }

    iterator insert(const_iterator pos, const T& value) {
        pos.node->value.~T();
        std::construct_at(std::addressof(pos.node->value), value);
        return sieveUp(pos);
    }

    iterator insert(const_iterator pos, T&& value) {
        pos.node->value.~T();
        std::construct_at(std::addressof(pos.node->value), std::move(value));
        return sieveUp(pos);
    }

    template <typename... Args>
        requires(std::constructible_from<T, Args...>)
    iterator emplace(Args&&... args) {
        auto node = std::make_shared<Node>(std::forward<Args...>(args...));
        return merge(node);
    }

    template <typename... Args>
        requires(std::constructible_from<T, Args...>)
    iterator emplace(const_iterator pos, Args&&... args) {
        pos.node->value.~T();
        std::construct_at(std::addressof(pos.node->value), std::forward<Args...>(args...));
        return sieveUp(pos);
    }

    iterator erase(const_iterator pos) noexcept {
        std::shared_ptr<Node> cur = pos.node;
        while (cur->parent) {
            std::swap(cur->value, cur->parent->value);
            cur = cur->parent;
        }
        std::shared_ptr<Node> extracted = extract(pos);
        return merge(extracted);
    }
};
