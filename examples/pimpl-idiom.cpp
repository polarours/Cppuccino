// examples/pimpl_idiom.cpp
// Demonstrates PImpl (Pointer to Implementation) idiom for ABI stability.
// Compile with: g++ -std=c++17 -o pimpl_example pimpl_idiom.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

// Forward declaration of implementation class
class WidgetImpl;

// Public interface using PImpl
class Widget {
public:
    Widget(const std::string& name, int value);
    ~Widget();
    
    // Rule of Five - only declarations in header
    Widget(const Widget& other);
    Widget& operator=(const Widget& other);
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;
    
    // Public interface
    std::string name() const;
    int value() const;
    void setValue(int value);
    void print() const;
    
    // Additional methods that don't expose implementation
    void addTag(const std::string& tag);
    std::vector<std::string> tags() const;

private:
    std::unique_ptr<WidgetImpl> impl_;
};

// Implementation class (defined in .cpp file in real project)
class WidgetImpl {
public:
    WidgetImpl(const std::string& name, int value) 
        : name_(name), value_(value) {
        std::cout << "[Impl] Created WidgetImpl: " << name_ << '\n';
    }
    
    ~WidgetImpl() {
        std::cout << "[Impl] Destroyed WidgetImpl: " << name_ << '\n';
    }
    
    WidgetImpl(const WidgetImpl& other) 
        : name_(other.name_), value_(other.value_), tags_(other.tags_) {
        std::cout << "[Impl] Copied WidgetImpl: " << name_ << '\n';
    }
    
    WidgetImpl& operator=(const WidgetImpl& other) {
        if (this != &other) {
            name_ = other.name_;
            value_ = other.value_;
            tags_ = other.tags_;
            std::cout << "[Impl] Copy assigned WidgetImpl: " << name_ << '\n';
        }
        return *this;
    }
    
    WidgetImpl(WidgetImpl&& other) noexcept 
        : name_(std::move(other.name_)), 
          value_(other.value_),
          tags_(std::move(other.tags_)) {
        other.value_ = 0;
        std::cout << "[Impl] Moved WidgetImpl: " << name_ << '\n';
    }
    
    WidgetImpl& operator=(WidgetImpl&& other) noexcept {
        if (this != &other) {
            name_ = std::move(other.name_);
            value_ = other.value_;
            tags_ = std::move(other.tags_);
            other.value_ = 0;
            std::cout << "[Impl] Move assigned WidgetImpl: " << name_ << '\n';
        }
        return *this;
    }
    
    std::string name() const { return name_; }
    int value() const { return value_; }
    void setValue(int value) { value_ = value; }
    
    void addTag(const std::string& tag) { tags_.push_back(tag); }
    std::vector<std::string> tags() const { return tags_; }
    
    void print() const {
        std::cout << "Widget: " << name_ << ", Value: " << value_;
        if (!tags_.empty()) {
            std::cout << ", Tags: [";
            for (size_t i = 0; i < tags_.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << tags_[i];
            }
            std::cout << "]";
        }
        std::cout << '\n';
    }

private:
    std::string name_;
    int value_;
    std::vector<std::string> tags_;
};

// Widget method implementations
Widget::Widget(const std::string& name, int value) 
    : impl_(std::make_unique<WidgetImpl>(name, value)) {}

Widget::~Widget() = default;

Widget::Widget(const Widget& other) 
    : impl_(std::make_unique<WidgetImpl>(*other.impl_)) {}

Widget& Widget::operator=(const Widget& other) {
    if (this != &other) {
        impl_ = std::make_unique<WidgetImpl>(*other.impl_);
    }
    return *this;
}

Widget::Widget(Widget&& other) noexcept = default;

Widget& Widget::operator=(Widget&& other) noexcept = default;

std::string Widget::name() const { return impl_->name(); }
int Widget::value() const { return impl_->value(); }
void Widget::setValue(int value) { impl_->setValue(value); }
void Widget::print() const { impl_->print(); }
void Widget::addTag(const std::string& tag) { impl_->addTag(tag); }
std::vector<std::string> Widget::tags() const { return impl_->tags(); }

// Demonstrate PImpl usage
void demonstratePImpl() {
    std::cout << "=== PImpl Idiom Demo ===\n\n";
    
    // Create widget
    Widget w1("First", 42);
    w1.print();
    
    // Copy widget
    Widget w2 = w1;
    w2.print();
    
    // Modify copy
    w2.setValue(100);
    w2.addTag("modified");
    std::cout << "\nAfter modification:\n";
    w1.print();
    w2.print();
    
    // Move widget
    Widget w3 = std::move(w2);
    std::cout << "\nAfter move:\n";
    w3.print();
}

// Demonstrate ABI stability benefit
void demonstrateABIStability() {
    std::cout << "\n=== ABI Stability Demo ===\n";
    
    // The implementation details are hidden
    // Changing WidgetImpl doesn't affect users of Widget
    // This is the key benefit of PImpl
    
    Widget widget("ABI Test", 123);
    widget.print();
    
    // Can add new methods to WidgetImpl without breaking ABI
    widget.addTag("stable");
    widget.addTag("abi");
    widget.print();
}

int main() {
    demonstratePImpl();
    demonstrateABIStability();
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
