#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <iostream>

namespace lock_free_queue {

template <typename T>
class LockFreeQueue {
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};
        explicit Node(T val) : data(std::move(val)) {}
    };

public:
    LockFreeQueue() : head_(new Node(T{})), tail_(head_.load()) {}

    ~LockFreeQueue() {
        while (auto node = head_.load()) {
            head_.store(node->next.load());
            delete node;
        }
    }

    void push(T value) {
        auto newNode = new Node(std::move(value));
        while (true) {
            Node* tail = tail_.load();
            Node* next = tail->next.load();
            if (tail == tail_.load()) {
                if (next == nullptr) {
                    if (tail->next.compare_exchange_weak(next, newNode)) {
                        tail_.compare_exchange_strong(tail, newNode);
                        return;
                    }
                } else {
                    tail_.compare_exchange_strong(tail, next);
                }
            }
        }
    }

    std::optional<T> pop() {
        while (true) {
            Node* head = head_.load();
            Node* tail = tail_.load();
            Node* next = head->next.load();
            if (head == head_.load()) {
                if (head == tail) {
                    if (next == nullptr) return std::nullopt;
                    tail_.compare_exchange_strong(tail, next);
                } else {
                    T value = std::move(next->data);
                    if (head_.compare_exchange_weak(head, next)) {
                        delete head;
                        return value;
                    }
                }
            }
        }
    }

    bool empty() const {
        return head_.load() == tail_.load() && head_.load()->next.load() == nullptr;
    }

private:
    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
};

} // namespace lock_free_queue
