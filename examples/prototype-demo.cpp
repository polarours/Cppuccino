// examples/prototype-demo.cpp
// Demonstrates the Prototype Pattern: clone existing objects instead of
// rebuilding them from scratch (document templates with attachments).
// Compile: g++ -std=c++20 -Wall -Wextra -o prototype-demo prototype-demo.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace prototype_demo {

class Attachment {
public:
    explicit Attachment(std::string name) : name_(std::move(name)) {}
    std::string name() const { return name_; }
private:
    std::string name_;
};

class Document {
public:
    Document(std::string title, std::string author)
        : title_(std::move(title)), author_(std::move(author)) {}

    // Deep-copy clone
    virtual std::unique_ptr<Document> clone() const = 0;
    virtual ~Document() = default;

    void addAttachment(const std::string& name) {
        attachments_.push_back(std::make_unique<Attachment>(name));
    }

    virtual void print() const {
        std::cout << "\"" << title_ << "\" by " << author_
                  << " [" << kind() << "]";
        if (!attachments_.empty()) {
            std::cout << " attachments:";
            for (const auto& a : attachments_) std::cout << " " << a->name();
        }
        std::cout << "\n";
    }

protected:
    Document(const Document& other)
        : title_(other.title_), author_(other.author_) {
        for (const auto& a : other.attachments_)
            attachments_.push_back(std::make_unique<Attachment>(a->name()));
    }

    virtual std::string kind() const = 0;

    std::string title_;
    std::string author_;
    std::vector<std::unique_ptr<Attachment>> attachments_;
};

class Report : public Document {
public:
    using Document::Document;
    std::unique_ptr<Document> clone() const override {
        return std::make_unique<Report>(*this);
    }
protected:
    std::string kind() const override { return "Report"; }
};

class Invoice : public Document {
public:
    using Document::Document;
    std::unique_ptr<Document> clone() const override {
        return std::make_unique<Invoice>(*this);
    }
protected:
    std::string kind() const override { return "Invoice"; }
};

} // namespace prototype_demo

int main() {
    using namespace prototype_demo;
    std::cout << "=== Prototype Pattern Demo ===\n\n";

    // Build the prototype once...
    Report proto("Q3 Engineering Report", "Alice");
    proto.addAttachment("metrics.csv");
    proto.addAttachment("chart.png");

    std::cout << "Prototype:\n";
    proto.print();

    // ...then clone it per reader, tweaking only what differs
    auto forAlice = proto.clone();
    auto forBob = proto.clone();

    std::cout << "\nClones (independent copies):\n";
    forAlice->print();
    forBob->print();

    // Mutating a clone must not touch the prototype
    forAlice->addAttachment("cover.png");
    std::cout << "\nAfter adding attachment to clone 1:\n";
    forAlice->print();
    proto.print();

    // A different prototype
    Invoice inv("Invoice #1001", "Bob");
    inv.addAttachment("receipt.pdf");
    std::cout << "\n";
    inv.print();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
