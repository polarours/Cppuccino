#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace type_erasure {

// 1) Hand-rolled type erasure (the "any callable with signature" pattern).
//    This is how std::function works under the hood: a small polymorphic
//    wrapper around a template-unique holder.
class AnyAction {
public:
    // Exclude AnyAction itself: without this, `AnyAction a2 = a1;` would
    // prefer the template ctor (exact match) over the copy ctor -> infinite
    // recursion inside Holder<AnyAction> construction.
    template <typename F,
              typename D = std::decay_t<F>,
              std::enable_if_t<!std::is_same_v<D, AnyAction>, int> = 0>
    AnyAction(F&& f)
        : holder_(std::make_unique<Holder<D>>(std::forward<F>(f))) {}

    // Rule of five: copyable via cloning the holder
    AnyAction(const AnyAction& other)
        : holder_(other.holder_ ? other.holder_->clone() : nullptr) {}
    AnyAction& operator=(const AnyAction& other) {
        if (this != &other)
            holder_ = other.holder_ ? other.holder_->clone() : nullptr;
        return *this;
    }
    AnyAction(AnyAction&&) noexcept = default;
    AnyAction& operator=(AnyAction&&) noexcept = default;

    void operator()() { holder_->invoke(); }
    explicit operator bool() const { return static_cast<bool>(holder_); }
    std::string label() const { return holder_ ? holder_->label() : ""; }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void invoke() = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
        virtual std::string label() const = 0;
    };

    template <typename F>
    struct Holder final : Concept {
        explicit Holder(F f) : fn(std::move(f)) {}
        void invoke() override { fn(); }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Holder<F>>(fn);
        }
        std::string label() const override { return typeid(F).name(); }
        F fn;
    };

    std::unique_ptr<Concept> holder_;
};

// 2) Type-erased value: store anything printable, report its type name.
class AnyPrintable {
public:
    // Same self-exclusion rule as AnyAction (avoid Model<AnyPrintable>).
    template <typename T,
              typename D = std::decay_t<T>,
              std::enable_if_t<!std::is_same_v<D, AnyPrintable>, int> = 0>
    AnyPrintable(T value)
        : self_(std::make_unique<Model<D>>(std::move(value))) {}

    // Value-like copy: clone the model (needed for vector init-lists too)
    AnyPrintable(const AnyPrintable& other)
        : self_(other.self_ ? other.self_->clone() : nullptr) {}
    AnyPrintable& operator=(const AnyPrintable& other) {
        if (this != &other)
            self_ = other.self_ ? other.self_->clone() : nullptr;
        return *this;
    }
    AnyPrintable(AnyPrintable&&) noexcept = default;
    AnyPrintable& operator=(AnyPrintable&&) noexcept = default;

    std::string str() const { return self_->str(); }
    std::string typeName() const { return self_->typeName(); }

private:
    struct Interface {
        virtual ~Interface() = default;
        virtual std::string str() const = 0;
        virtual std::string typeName() const = 0;
        virtual std::unique_ptr<Interface> clone() const = 0;
    };

    template <typename T>
    struct Model final : Interface {
        explicit Model(T v) : value(std::move(v)) {}
        std::string str() const override { return toText(value); }
        std::string typeName() const override { return typeid(T).name(); }
        std::unique_ptr<Interface> clone() const override {
            return std::make_unique<Model<T>>(value);
        }
        T value;
    };

    static std::string toText(const std::string& v) { return v; }
    static std::string toText(const char* v) { return v; }
    static std::string toText(bool v) { return v ? "true" : "false"; }
    template <typename T>
    static auto toText(const T& v) -> decltype(std::to_string(v)) {
        return std::to_string(v);
    }

    std::unique_ptr<Interface> self_;
};

}  // namespace type_erasure
