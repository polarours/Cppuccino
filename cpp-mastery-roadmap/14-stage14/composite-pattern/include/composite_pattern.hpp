#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

namespace composite_pattern {

// --- Component Interface ---

class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    virtual void print(int indent = 0) const = 0;
    virtual std::string getName() const = 0;
    virtual std::size_t getSize() const = 0;
};

// --- Leaf ---

class File : public FileSystemComponent {
public:
    File(const std::string& name, std::size_t size)
        : name_(name), size_(size) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "File: " << name_
                  << " (" << size_ << " bytes)\n";
    }

    std::string getName() const override { return name_; }
    std::size_t getSize() const override { return size_; }

private:
    std::string name_;
    std::size_t size_;
};

// --- Composite ---

class Directory : public FileSystemComponent {
public:
    explicit Directory(const std::string& name) : name_(name) {}

    void add(std::shared_ptr<FileSystemComponent> component) {
        children_.push_back(component);
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Directory: " << name_ << "\n";
        for (const auto& child : children_) {
            child->print(indent + 2);
        }
    }

    std::string getName() const override { return name_; }

    std::size_t getSize() const override {
        std::size_t total = 0;
        for (const auto& child : children_) {
            total += child->getSize();
        }
        return total;
    }

    const std::vector<std::shared_ptr<FileSystemComponent>>& getChildren() const {
        return children_;
    }

private:
    std::string name_;
    std::vector<std::shared_ptr<FileSystemComponent>> children_;
};

} // namespace composite_pattern
