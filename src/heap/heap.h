#ifndef CPP_PILOT_HSE_HEAP_H
#define CPP_PILOT_HSE_HEAP_H
#include <memory>
#include <random>
#include <functional>
#include <exception>

namespace {
std::mt19937 mt_rand(time(0));  // NOLINT
}

template <typename T>
class Heap {
    struct HeapNode {
        using HeapPtr = std::unique_ptr<HeapNode>;
        T val_;
        HeapPtr l_;
        HeapPtr r_;
        explicit HeapNode(T val) : val_(val) {
        }

        void Extract(HeapNode&& other) {
            std::swap(val_, other.val_);
            std::swap(l_, other.l_);
            std::swap(r_, other.r_);
        }
        HeapNode(HeapNode&& other) {
            Extract(std::move(other));
        }
        HeapNode& operator=(HeapNode&& other) {
            Extract(std::move(other));
            return *this;
        }
    };

    using HeapPtr = std::unique_ptr<HeapNode>;
    using Operation = bool(T, T);
    HeapPtr root_;
    std::function<Operation> comp_;
    size_t size_ = 0;

    HeapPtr Merge(HeapPtr t1, HeapPtr t2) {
        if (!t1) {
            return t2;
        }
        if (!t2) {
            return t1;
        }
        if (!comp_(t1->val_, t2->val_)) {
            std::swap(t1, t2);
        }
        if (mt_rand() & 1) {
            std::swap(t1->l_, t1->r_);
        }
        t1->l_ = Merge(std::move(t1->l_), std::move(t2));
        return std::move(t1);
    }

    static bool Greater(const T& val1, const T& val2) {
        return val1 > val2;
    }

public:
    explicit Heap(std::function<Operation> comp = Greater) : comp_(comp), size_(0) {
    }
    void Push(T val) {
        ++size_;
        HeapPtr ptr = std::unique_ptr<HeapNode>(new HeapNode(val));
        auto tmp = Merge(std::move(ptr), std::move(root_));
        root_ = std::move(tmp);
    }

    size_t Size() {
        return size_;
    }

    const T& Top() {
        if (size_ == 0) {
            throw std::out_of_range("accessing null element");
        }
        return root_->val_;
    }

    void Pop() {
        if (size_ == 0) {
            throw std::out_of_range("popping null element");
        }
        --size_;
        root_ = Merge(std::move(root_->l_), std::move(root_->r_));
    }
};
#endif
