#include <memory>
#include <string>
#include <iostream>

// std::shared_ptr - shared ownership smart pointer
// std::weak_ptr - non-owning reference

class Person {
public:
    Person(std::string name) : name_(std::move(name)) {
        std::cout << "Person created: " << name_ << "\n";
    }

    ~Person() {
        std::cout << "Person destroyed: " << name_ << "\n";
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
};

class Couple {
public:
    Couple(std::shared_ptr<Person> a, std::shared_ptr<Person> b)
        : a_(std::move(a)), b_(std::move(b)) {}

    void describe() const {
        std::cout << "Couple: " << a_->name() << " & " << b_->name() << "\n";
    }

    std::size_t useCount() const { return a_.use_count(); }

private:
    std::shared_ptr<Person> a_;
    std::shared_ptr<Person> b_;
};

int main() {
    std::cout << "=== std::shared_ptr ===\n\n";

    // Basic usage
    auto alice = std::make_shared<Person>("Alice");
    auto bob = std::make_shared<Person>("Bob");

    std::cout << "Alice use_count: " << alice.use_count() << "\n";  // 1

    // Shared ownership
    {
        Couple couple(alice, bob);
        std::cout << "Alice use_count: " << alice.use_count() << "\n";  // 2
        couple.describe();
    }
    // Couple destroyed, use_count back to 1
    std::cout << "Alice use_count: " << alice.use_count() << "\n";  // 1

    // weak_ptr prevents circular references
    std::weak_ptr<Person> weakAlice = alice;
    std::cout << "weak_ptr expired: " << weakAlice.expired() << "\n";  // false

    // Lock weak_ptr to access object
    if (auto locked = weakAlice.lock()) {
        std::cout << "Locked: " << locked->name() << "\n";
    }

    alice.reset();
    bob.reset();
    // Alice and Bob destroyed here

    std::cout << "weak_ptr expired: " << weakAlice.expired() << "\n";  // true

    return 0;
}
