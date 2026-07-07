#pragma once

#include <cstddef>
#include <functional>
#include <utility>

namespace smart_ptr {

template <typename T>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) noexcept;
    ~UniquePtr();

    UniquePtr(UniquePtr&& other) noexcept;
    UniquePtr& operator=(UniquePtr&& other) noexcept;

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    T& operator*() const;
    T* operator->() const;
    T* get() const;

    T* release() noexcept;
    void reset(T* ptr = nullptr) noexcept;

    explicit operator bool() const noexcept;

    bool operator==(const UniquePtr& other) const noexcept;
    bool operator!=(const UniquePtr& other) const noexcept;
    bool operator==(std::nullptr_t) const noexcept;
    bool operator!=(std::nullptr_t) const noexcept;

private:
    T* ptr_;
};

template <typename T>
class SharedPtr {
public:
    explicit SharedPtr(T* ptr = nullptr);
    ~SharedPtr();

    SharedPtr(const SharedPtr& other);
    SharedPtr& operator=(const SharedPtr& other);

    SharedPtr(SharedPtr&& other) noexcept;
    SharedPtr& operator=(SharedPtr&& other) noexcept;

    T& operator*() const;
    T* operator->() const;
    T* get() const;

    std::size_t use_count() const;
    bool unique() const;

    void reset(T* ptr = nullptr);

    explicit operator bool() const noexcept;

    bool operator==(const SharedPtr& other) const noexcept;
    bool operator!=(const SharedPtr& other) const noexcept;
    bool operator==(std::nullptr_t) const noexcept;
    bool operator!=(std::nullptr_t) const noexcept;

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

private:
    struct ControlBlock {
        T* ptr;
        std::size_t strong_count;
        std::size_t weak_count;

        explicit ControlBlock(T* p);
        void addStrong();
        void releaseStrong();
        void addWeak();
        void releaseWeak();
    };

    T* ptr_;
    ControlBlock* block_;

    explicit SharedPtr(T* ptr, ControlBlock* block);
};

template <typename T>
class WeakPtr {
public:
    WeakPtr() noexcept;
    WeakPtr(const SharedPtr<T>& shared) noexcept;

    WeakPtr(const WeakPtr& other);
    WeakPtr& operator=(const WeakPtr& other);

    WeakPtr(WeakPtr&& other) noexcept;
    WeakPtr& operator=(WeakPtr&& other) noexcept;

    bool expired() const noexcept;
    SharedPtr<T> lock() const noexcept;

    std::size_t use_count() const noexcept;

    void reset() noexcept;

    template <typename U>
    friend class WeakPtr;

private:
    typename SharedPtr<T>::ControlBlock* block_;
};

// --- UniquePtr implementation ---

template <typename T>
UniquePtr<T>::UniquePtr(T* ptr) noexcept : ptr_(ptr) {}

template <typename T>
UniquePtr<T>::~UniquePtr() {
    delete ptr_;
}

template <typename T>
UniquePtr<T>::UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
        delete ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T>
T& UniquePtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* UniquePtr<T>::operator->() const {
    return ptr_;
}

template <typename T>
T* UniquePtr<T>::get() const {
    return ptr_;
}

template <typename T>
T* UniquePtr<T>::release() noexcept {
    T* temp = ptr_;
    ptr_ = nullptr;
    return temp;
}

template <typename T>
void UniquePtr<T>::reset(T* ptr) noexcept {
    if (ptr_ != ptr) {
        delete ptr_;
        ptr_ = ptr;
    }
}

template <typename T>
UniquePtr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
bool UniquePtr<T>::operator==(const UniquePtr& other) const noexcept {
    return ptr_ == other.ptr_;
}

template <typename T>
bool UniquePtr<T>::operator!=(const UniquePtr& other) const noexcept {
    return ptr_ != other.ptr_;
}

template <typename T>
bool UniquePtr<T>::operator==(std::nullptr_t) const noexcept {
    return ptr_ == nullptr;
}

template <typename T>
bool UniquePtr<T>::operator!=(std::nullptr_t) const noexcept {
    return ptr_ != nullptr;
}

// --- SharedPtr implementation ---

template <typename T>
SharedPtr<T>::ControlBlock::ControlBlock(T* p)
    : ptr(p), strong_count(1), weak_count(0) {}

template <typename T>
void SharedPtr<T>::ControlBlock::addStrong() {
    strong_count++;
}

template <typename T>
void SharedPtr<T>::ControlBlock::releaseStrong() {
    if (--strong_count == 0) {
        delete ptr;
        ptr = nullptr;
        if (weak_count == 0) {
            delete this;
        }
    }
}

template <typename T>
void SharedPtr<T>::ControlBlock::addWeak() {
    weak_count++;
}

template <typename T>
void SharedPtr<T>::ControlBlock::releaseWeak() {
    if (--weak_count == 0 && strong_count == 0) {
        delete this;
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(T* ptr)
    : ptr_(ptr), block_(ptr ? new ControlBlock(ptr) : nullptr) {}

template <typename T>
SharedPtr<T>::SharedPtr(T* ptr, ControlBlock* block)
    : ptr_(ptr), block_(block) {}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (block_) {
        block_->releaseStrong();
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other)
    : ptr_(other.ptr_), block_(other.block_) {
    if (block_) {
        block_->addStrong();
    }
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
    if (this != &other) {
        if (block_) {
            block_->releaseStrong();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            block_->addStrong();
        }
    }
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : ptr_(other.ptr_), block_(other.block_) {
    other.ptr_ = nullptr;
    other.block_ = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept {
    if (this != &other) {
        if (block_) {
            block_->releaseStrong();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }
    return *this;
}

template <typename T>
T& SharedPtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* SharedPtr<T>::operator->() const {
    return ptr_;
}

template <typename T>
T* SharedPtr<T>::get() const {
    return ptr_;
}

template <typename T>
std::size_t SharedPtr<T>::use_count() const {
    return block_ ? block_->strong_count : 0;
}

template <typename T>
bool SharedPtr<T>::unique() const {
    return use_count() == 1;
}

template <typename T>
void SharedPtr<T>::reset(T* ptr) {
    if (ptr_ != ptr) {
        if (block_) {
            block_->releaseStrong();
        }
        ptr_ = ptr;
        block_ = ptr ? new ControlBlock(ptr) : nullptr;
    }
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
bool SharedPtr<T>::operator==(const SharedPtr& other) const noexcept {
    return ptr_ == other.ptr_;
}

template <typename T>
bool SharedPtr<T>::operator!=(const SharedPtr& other) const noexcept {
    return ptr_ != other.ptr_;
}

template <typename T>
bool SharedPtr<T>::operator==(std::nullptr_t) const noexcept {
    return ptr_ == nullptr;
}

template <typename T>
bool SharedPtr<T>::operator!=(std::nullptr_t) const noexcept {
    return ptr_ != nullptr;
}

// --- WeakPtr implementation ---

template <typename T>
WeakPtr<T>::WeakPtr() noexcept : block_(nullptr) {}

template <typename T>
WeakPtr<T>::WeakPtr(const SharedPtr<T>& shared) noexcept
    : block_(shared.block_) {
    if (block_) {
        block_->addWeak();
    }
}

template <typename T>
WeakPtr<T>::WeakPtr(const WeakPtr& other) : block_(other.block_) {
    if (block_) {
        block_->addWeak();
    }
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) {
    if (this != &other) {
        if (block_) {
            block_->releaseWeak();
        }
        block_ = other.block_;
        if (block_) {
            block_->addWeak();
        }
    }
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept : block_(other.block_) {
    other.block_ = nullptr;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept {
    if (this != &other) {
        if (block_) {
            block_->releaseWeak();
        }
        block_ = other.block_;
        other.block_ = nullptr;
    }
    return *this;
}

template <typename T>
bool WeakPtr<T>::expired() const noexcept {
    return !block_ || block_->strong_count == 0;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
    if (expired()) {
        return SharedPtr<T>();
    }
    block_->addStrong();
    return SharedPtr<T>(block_->ptr, block_);
}

template <typename T>
std::size_t WeakPtr<T>::use_count() const noexcept {
    return block_ ? block_->strong_count : 0;
}

template <typename T>
void WeakPtr<T>::reset() noexcept {
    if (block_) {
        block_->releaseWeak();
        block_ = nullptr;
    }
}

} // namespace smart_ptr
