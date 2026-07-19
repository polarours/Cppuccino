#pragma once

#include <string>
#include <iostream>

namespace deducing_this {

class Builder {
public:
    template <typename Self>
    auto&& withName(this Self&& self, const std::string& name) {
        self.name_ = name;
        return std::forward<Self>(self);
    }

    template <typename Self>
    auto&& withAge(this Self&& self, int age) {
        self.age_ = age;
        return std::forward<Self>(self);
    }

    template <typename Self>
    auto&& withEmail(this Self&& self, const std::string& email) {
        self.email_ = email;
        return std::forward<Self>(self);
    }

    std::string describe() const {
        return "Person(" + name_ + ", " + std::to_string(age_) + ", " + email_ + ")";
    }

    const std::string& name() const { return name_; }
    int age() const { return age_; }
    const std::string& email() const { return email_; }

private:
    std::string name_;
    int age_ = 0;
    std::string email_;
};

class Counter {
public:
    template <typename Self>
    auto&& increment(this Self&& self) {
        ++self.value_;
        return std::forward<Self>(self);
    }

    template <typename Self>
    auto&& add(this Self&& self, int n) {
        self.value_ += n;
        return std::forward<Self>(self);
    }

    int value() const { return value_; }

private:
    int value_ = 0;
};

} // namespace deducing_this
