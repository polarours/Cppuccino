#include <memory>
#include <string>
#include <iostream>

// std::unique_ptr - exclusive ownership smart pointer
// std::make_unique - factory function (C++14)

class Resource {
public:
    explicit Resource(std::string name) : name_(std::move(name)) {
        std::cout << "Resource created: " << name_ << "\n";
    }

    ~Resource() {
        std::cout << "Resource destroyed: " << name_ << "\n";
    }

    const std::string& name() const { return name_; }

    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

private:
    std::string name_;
};

std::unique_ptr<Resource> createResource(const std::string& name) {
    return std::make_unique<Resource>(name);
}

int main() {
    std::cout << "=== std::unique_ptr ===\n\n";

    // Basic usage
    auto ptr1 = std::make_unique<Resource>("Widget");
    std::cout << "ptr1 name: " << ptr1->name() << "\n";

    // Transfer ownership
    auto ptr2 = std::move(ptr1);
    // ptr1 is now nullptr
    std::cout << "ptr1 valid: " << (ptr1 != nullptr) << "\n";
    std::cout << "ptr2 valid: " << (ptr2 != nullptr) << "\n";

    // Factory function
    auto ptr3 = createResource("Gadget");
    std::cout << "ptr3 name: " << ptr3->name() << "\n";

    // Array allocation
    auto arr = std::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) arr[i] = i * 10;

    std::cout << "Array: ";
    for (int i = 0; i < 5; ++i) std::cout << arr[i] << " ";
    std::cout << "\n";

    // Custom deleter
    auto customDeleter = [](Resource* r) {
        std::cout << "Custom delete: " << r->name() << "\n";
        delete r;
    };

    {
        std::unique_ptr<Resource, decltype(customDeleter)> ptr(
            new Resource("Custom"), customDeleter);
        // Destroyed when scope ends
    }

    // Release ownership
    Resource* raw = ptr2.release();
    std::cout << "Released: " << raw->name() << "\n";
    delete raw;  // Manual cleanup required

    // Reset
    ptr3.reset();
    std::cout << "ptr3 after reset: " << (ptr3 != nullptr) << "\n";

    return 0;
}
