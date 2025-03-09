#pragma once

#include <cassert>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

template <typename T, typename Cmp = std::less<T>>
    requires(std::invocable<Cmp, const T&, const T&>)
class BinomialHeap {
protected:
    using Deg = unsigned;
    static constexpr Deg MAX_DEG = std::numeric_limits<Deg>::max();

    struct Node final {
        std::shared_ptr<Node> leftmost_child = nullptr;
        std::shared_ptr<Node> parent = nullptr;
        std::shared_ptr<Node> neighbour = nullptr;
        Deg deg = 0;
        T value;

        std::shared_ptr<Node> clone(std::shared_ptr<Node> new_parent) {
            return std::make_shared<Node>(leftmost_child.clone(), new_parent, neighbour.clone(),
                                          deg, value);
        }

        Node() = default;
        Node(std::shared_ptr<Node> child, std::shared_ptr<Node> parent,
             std::shared_ptr<Node> neighbour, unsigned deg, const T& value)
            : leftmost_child{child}, parent{parent}, neighbour{neighbour}, deg{deg}, value{value} {}

        template <typename... Args>
            requires(std::constructible_from<T, Args...>)
        Node(Args&&... args) : value{std::forward<Args>(args)...} {}
    };

    class Iterator final {
        friend BinomialHeap;

        std::shared_ptr<Node> node;

        explicit Iterator(std::shared_ptr<Node> node) : node{node} {}

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

        pointer operator->() const noexcept {
            return std::addressof(node->value);
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

protected:
    iterator makeIterator(std::shared_ptr<Node> n) {
        return iterator{n};
    }

    std::shared_ptr<Node> nodeFromIter(iterator it) {
        return it.node;
    }

    std::shared_ptr<Node> merge(std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs) {
        assert(lhs->deg == rhs->deg);

        if (!comp_(lhs->value, rhs->value)) {
            std::swap(lhs, rhs);
        }

        assert(!lhs->neighbour);

        rhs->parent = lhs;
        rhs->neighbour = lhs->leftmost_child;
        lhs->leftmost_child = rhs;
        lhs->deg += 1;
        return lhs;
    }

    iterator merge(std::shared_ptr<Node> node) {
        if (!node) {
            return end();
        }

        if (!root_) {
            root_ = node;
            return iterator{root_};
        }

        std::shared_ptr<Node> result = nullptr;
        std::shared_ptr<Node> res_end = nullptr;

        std::shared_ptr<Node> carry = nullptr;
        std::shared_ptr<Node> i = root_;
        std::shared_ptr<Node> j = node;

        auto get_deg = [](const Node* p) { return p ? p->deg : MAX_DEG; };
        auto append = [&result, &res_end](std::shared_ptr<Node> n) {
            assert(!n->neighbour);

            if (!result) {
                result = res_end = n;
            } else {
                res_end->neighbour = n;
                res_end = n;
            }
        };

        while (i || j || carry) {
            Deg min_deg = std::min({get_deg(i.get()), get_deg(j.get()), get_deg(carry.get())});
            bool t1 = i && i->deg == min_deg;
            bool t2 = j && j->deg == min_deg;
            bool tc = carry && carry->deg == min_deg;

            if (t1 && t2 && tc) {
                append(carry);
                std::shared_ptr<Node> tmp1 = i->neighbour;
                std::shared_ptr<Node> tmp2 = j->neighbour;
                i->neighbour = nullptr;
                j->neighbour = nullptr;
                carry = merge(i, j);
                i = tmp1;
                j = tmp2;
            } else if (t1 && t2) {
                std::shared_ptr<Node> tmp1 = i->neighbour;
                std::shared_ptr<Node> tmp2 = j->neighbour;
                i->neighbour = nullptr;
                j->neighbour = nullptr;
                carry = merge(i, j);
                i = tmp1;
                j = tmp2;
            } else if (t1 && tc) {
                std::shared_ptr<Node> tmp = i->neighbour;
                i->neighbour = nullptr;
                carry = merge(i, carry);
                i = tmp;
            } else if (t2 && tc) {
                std::shared_ptr<Node> tmp = j->neighbour;
                j->neighbour = nullptr;
                carry = merge(j, carry);
                j = tmp;
            } else if (t1) {
                std::shared_ptr<Node> tmp = i->neighbour;
                i->neighbour = nullptr;
                append(i);
                i = tmp;
            } else if (t2) {
                std::shared_ptr<Node> tmp = j->neighbour;
                j->neighbour = nullptr;
                append(j);
                j = tmp;
            } else if (tc) {
                append(carry);
                carry = nullptr;
            }
        }

        root_ = result;
        return iterator{node};
    }

    static std::shared_ptr<Node> heapifyNeighbourhood(std::shared_ptr<Node> head) {
        std::shared_ptr<Node> prev = nullptr;
        std::shared_ptr<Node> next = nullptr;
        std::shared_ptr<Node> cur = head;
        while (cur) {
            cur->parent = nullptr;
            next = std::exchange(cur->neighbour, prev);
            prev = std::exchange(cur, next);
        }

        return prev;
    }

    iterator sieveUp(const_iterator pos) {
        std::shared_ptr<Node> cur = pos.node;
        while (cur && cur->parent && comp_(cur->value, cur->parent->value)) {
            std::swap(cur->value, cur->parent->value);
            cur = cur->parent;
        }

        return iterator{cur};
    }

    std::shared_ptr<Node> extract(const_iterator pos) {
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

        if (res) {
            res->parent = nullptr;
        }
        return res;
    }

    std::shared_ptr<Node> root_ = nullptr;
    Cmp comp_{};
    size_type size_ = 0;

public:
    BinomialHeap() = default;
    BinomialHeap(const Cmp& comp) : comp_{comp} {}

    BinomialHeap(const BinomialHeap& other)
        : root_{other.root_.clone()}, comp_{other.comp_}, size_{other.size_} {}
    BinomialHeap& operator=(const BinomialHeap& other) {
        BinomialHeap heap{other};
        swap(heap);
        return *this;
    }

    BinomialHeap(BinomialHeap&& other) noexcept {
        swap(other);
    }

    BinomialHeap& operator=(BinomialHeap&& other) noexcept {
        swap(other);
        return *this;
    }

    void merge(BinomialHeap&& other) {
        merge(other.root_);
        size_ += other.size_;
    }

    void swap(BinomialHeap& other) {
        std::swap(root_, other.root_);
        std::swap(comp_, other.comp_);
        std::swap(size_, other.size_);
    }

    bool empty() const noexcept {
        return root_ == nullptr;
    }

    size_type size() const noexcept {
        return size_;
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

    const_iterator top() const {
        if (!root_) {
            return cend();
        }

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

    void pop() {
        assert(root_);
        assert(size_ > 0);

        std::shared_ptr<Node> extracted = extract(top());
        merge(heapifyNeighbourhood(extracted));
        size_ -= 1;
    }

    iterator insert(const T& value) {
        size_ += 1;
        auto node = std::make_shared<Node>(value);
        return merge(node);
    }

    iterator insert(T&& value) {
        size_ += 1;
        auto node = std::make_shared<Node>(std::move(value));
        return merge(node);
    }

    iterator insert(const_iterator pos, const T& value) {
        if (pos == cend()) {
            return insert(value);
        }

        auto addr = std::addressof(pos.node->value);
        std::destroy_at(addr);
        std::construct_at(addr, value);
        return sieveUp(pos);
    }

    iterator insert(const_iterator pos, T&& value) {
        if (pos == cend()) {
            return insert(std::move(value));
        }

        auto addr = std::addressof(pos.node->value);
        std::destroy_at(addr);
        std::construct_at(addr, std::move(value));
        return sieveUp(pos);
    }

    template <typename... Args>
        requires(std::constructible_from<T, Args...>)
    iterator emplace(Args&&... args) {
        size_ += 1;
        auto node = std::make_shared<Node>(std::forward<Args>(args)...);
        return merge(node);
    }

    template <typename... Args>
        requires(std::constructible_from<T, Args...>)
    iterator emplaceHint(const_iterator pos, Args&&... args) {
        if (pos == end()) {
            return emplace(std::forward<Args>(args)...);
        }

        auto addr = std::addressof(pos.node->value);
        std::destroy_at(addr);
        std::construct_at(addr, std::forward<Args>(args)...);
        return sieveUp(pos);
    }

    iterator erase(const_iterator pos) {
        if (pos == cend()) {
            return cend();
        }

        size_ -= 1;
        std::shared_ptr<Node> cur = pos.node;
        while (cur->parent) {
            std::swap(cur->value, cur->parent->value);
            cur = cur->parent;
        }
        std::shared_ptr<Node> extracted = extract(pos);
        return merge(heapifyNeighbourhood(extracted));
    }
};
