// examples/composite-demo.cpp
// Demonstrates the Composite Pattern: treat individual objects and
// compositions uniformly (a file system tree).
// Compile: g++ -std=c++20 -Wall -Wextra -o composite-demo composite-demo.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace composite_demo {

class FileSystemNode {
public:
    explicit FileSystemNode(std::string name) : name_(std::move(name)) {}
    virtual ~FileSystemNode() = default;
    virtual long size() const = 0;
    virtual void print(int indent = 0) const = 0;
    const std::string& name() const { return name_; }

protected:
    std::string name_;
    static void pad(int indent) {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    }
};

// Leaf
class File : public FileSystemNode {
public:
    File(std::string name, long bytes) : FileSystemNode(std::move(name)), bytes_(bytes) {}
    long size() const override { return bytes_; }
    void print(int indent) const override {
        pad(indent);
        std::cout << name_ << " (" << bytes_ << " bytes)\n";
    }
private:
    long bytes_;
};

// Composite
class Directory : public FileSystemNode {
public:
    explicit Directory(std::string name) : FileSystemNode(std::move(name)) {}

    void add(std::unique_ptr<FileSystemNode> child) {
        children_.push_back(std::move(child));
    }

    long size() const override {
        long total = 0;
        for (const auto& c : children_) total += c->size();
        return total;
    }

    void print(int indent) const override {
        pad(indent);
        std::cout << name_ << "/ (" << size() << " bytes)\n";
        for (const auto& c : children_) c->print(indent + 1);
    }

private:
    std::vector<std::unique_ptr<FileSystemNode>> children_;
};

} // namespace composite_demo

int main() {
    using namespace composite_demo;
    std::cout << "=== Composite Pattern Demo ===\n\n";

    auto root = std::make_unique<Directory>("root");

    auto src = std::make_unique<Directory>("src");
    src->add(std::make_unique<File>("main.cpp", 4096));
    src->add(std::make_unique<File>("util.cpp", 2048));

    auto docs = std::make_unique<Directory>("docs");
    docs->add(std::make_unique<File>("readme.md", 1024));

    root->add(std::move(src));
    root->add(std::move(docs));
    root->add(std::make_unique<File>("Makefile", 512));

    // Same interface on leaves and composites
    root->print(0);
    std::cout << "\nTotal size: " << root->size() << " bytes\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
