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
        // The chain of retired nodes is kept apart from the queue chain.
        // A retired node may still be read by a thread that is behind, so
        // its own next pointer has to stay untouched.
        Node* retired_next = nullptr;
        explicit Node(T val) : data(std::move(val)) {}
    };

public:
    LockFreeQueue() : head_(new Node(T{})), tail_(head_.load()) {}

    ~LockFreeQueue() {
        // The nodes still in the queue, and the ones that were popped.
        // The two sets do not overlap: a node is either the current head
        // or a head that has been retired.
        Node* node = head_.load();
        while (node) {
            Node* next = node->next.load();
            delete node;
            node = next;
        }
        node = retired_.load();
        while (node) {
            Node* next = node->retired_next;
            delete node;
            node = next;
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
                    if (head_.compare_exchange_weak(head, next)) {
                        // The payload is taken out only now, after the head
                        // has actually moved. Two threads can reach this
                        // point with the same next node, and only the one
                        // whose exchange succeeds owns the payload. Taking
                        // it out earlier would let both threads move from
                        // the same object at the same time.
                        T value = std::move(next->data);
                        // The old head is not freed here. A thread that is
                        // behind may still be reading its next pointer, and
                        // freeing it would pull the ground from under that
                        // read. It is retired instead, and the destructor
                        // frees the retired nodes. A production queue would
                        // use hazard pointers or epoch-based reclamation to
                        // release them earlier than that.
                        retire(head);
                        return value;
                    }
                }
            }
        }
    }

    bool empty() const {
        Node* head = head_.load();
        return head == tail_.load() && head->next.load() == nullptr;
    }

private:
    // Puts a node on the list of retired nodes. The list is only walked
    // by the destructor, so the nodes it holds stay valid for as long as
    // the queue is alive.
    void retire(Node* node) {
        Node* head = retired_.load(std::memory_order_relaxed);
        do {
            node->retired_next = head;
        } while (!retired_.compare_exchange_weak(head, node,
                                                std::memory_order_release,
                                                std::memory_order_relaxed));
    }

    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
    std::atomic<Node*> retired_{nullptr};
};

} // namespace lock_free_queue
