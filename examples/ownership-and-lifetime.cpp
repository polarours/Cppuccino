// examples/ownership_and_lifetime.cpp
// Demonstrates different ownership semantics in C++17 and their impact on object lifetime.
// Compile with: g++ -std=c++17 -o ownership_and_lifetime ownership_and_lifetime.cpp

#include <iostream>
#include <memory>
#include <string>

class Resource {
public:
explicit Resource(std::string name)
    : name_(std::move(name)) 
{
    std::cout << "[Acquire] Resource: " << name_ << " @ " << this << '\n';
}

~Resource() {
    std::cout << "[Release] Resource: " << name_ << " @ " << this << '\n';
}

void use() const {
    std::cout << "[Use] Resource: " << name_ << " @ " << this << '\n';
}

private:
    std::string name_;
};

std::unique_ptr<Resource> createUniqueResource() {
    return std::make_unique<Resource>("unique_resource");
}

void consumeUnique(std::unique_ptr<Resource> res) {
    std::cout << "[consumeUnique] owns resource @ " << res.get() << '\n';

    res->use();
}

void uniqueOwnershipScenario() {
    auto res = createUniqueResource();

    std::cout << "[Caller] owns resource @ " << res.get() << '\n';

    consumeUnique(std::move(res));
    if (!res) {
        std::cout << "[Caller] unique_ptr is now empty\n";
    }
}

void sharedOwnershipScenario() {
    auto res = std::make_shared<Resource>("shared_resource");

    std::cout << "[Owner A] resource @ "
              << res.get()
              << ", use_count = "
              << res.use_count() << '\n';

    {
        auto another = res;
        std::cout << "[Owner B] resource @ "
                  << another.get()
                  << ", use_count = "
                  << another.use_count() << '\n';

        another->use();
    }

    std::cout << "[Owner A] after inner scope, use_count = " << res.use_count() << '\n';
    res->use();
}

void observeResource(const Resource* res) {
    std::cout << "[Observer] sees resource @ " << res << '\n';

    if (res) {
        res->use();
    }
}

void observerScenario() {
    auto owner = std::make_unique<Resource>("observer_resource");
    std::cout << "[Owner] resource @ " << owner.get() << '\n';

    observeResource(owner.get());
}

int main() {
    std::cout << "\n=== Unique Ownership ===\n";
    uniqueOwnershipScenario();

    std::cout << "\n=== Shared Ownership ===\n";
    sharedOwnershipScenario();

    std::cout << "\n=== Observer Ownership ===\n";
    observerScenario();

    std::cout << "\n=== End of Program ===\n";
    return 0;
}
