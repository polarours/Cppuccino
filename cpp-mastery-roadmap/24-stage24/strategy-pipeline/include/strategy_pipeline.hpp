#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cctype>
#include <iostream>

namespace strategy_pipeline {

class Transform {
public:
    virtual ~Transform() = default;
    virtual std::string process(const std::string& input) = 0;
    virtual std::string name() const = 0;
};

class UpperTransform : public Transform {
public:
    std::string process(const std::string& input) override {
        std::string result = input;
        for (auto& c : result) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        return result;
    }
    std::string name() const override { return "Upper"; }
};

class TrimTransform : public Transform {
public:
    std::string process(const std::string& input) override {
        auto start = input.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        auto end = input.find_last_not_of(" \t\n\r");
        return input.substr(start, end - start + 1);
    }
    std::string name() const override { return "Trim"; }
};

class PrefixTransform : public Transform {
public:
    explicit PrefixTransform(std::string prefix) : prefix_(std::move(prefix)) {}

    std::string process(const std::string& input) override {
        return prefix_ + input;
    }
    std::string name() const override { return "Prefix"; }

private:
    std::string prefix_;
};

class Pipeline {
public:
    void add(std::shared_ptr<Transform> transform) {
        transforms_.push_back(std::move(transform));
    }

    std::string execute(const std::string& input) {
        std::string result = input;
        for (auto& t : transforms_) {
            result = t->process(result);
        }
        return result;
    }

    std::size_t size() const { return transforms_.size(); }

private:
    std::vector<std::shared_ptr<Transform>> transforms_;
};

} // namespace strategy_pipeline
