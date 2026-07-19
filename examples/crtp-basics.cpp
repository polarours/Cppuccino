#include <iostream>
#include <string>
#include <type_traits>

// --- Counter via CRTP ---
template <typename Derived>
struct Counter {
    static int instances;
    Counter() { ++instances; }
    Counter(const Counter&) { ++instances; }
    ~Counter() { --instances; }
};

template <typename Derived>
int Counter<Derived>::instances = 0;

struct Dog : Counter<Dog> { std::string name; };
struct Cat : Counter<Cat> { std::string name; };

// --- Printable via CRTP ---
template <typename Derived>
struct Printable {
    void print() const {
        static_cast<const Derived*>(this)->doPrint();
    }
};

struct User : Printable<User> {
    std::string name;
    int age;
    User(std::string n, int a) : name(std::move(n)), age(a) {}
    void doPrint() const {
        std::cout << "User(" << name << ", " << age << ")\n";
    }
};

struct Product : Printable<Product> {
    std::string name;
    double price;
    Product(std::string n, double p) : name(std::move(n)), price(p) {}
    void doPrint() const {
        std::cout << "Product(" << name << ", $" << price << ")\n";
    }
};

int main() {
    std::cout << "=== CRTP Pattern ===\n\n";

    Dog d1, d2;
    Cat c1;
    std::cout << "Dogs: " << Counter<Dog>::instances << "\n";
    std::cout << "Cats: " << Counter<Cat>::instances << "\n";

    {
        Dog d3;
        std::cout << "Dogs after add: " << Counter<Dog>::instances << "\n";
    }
    std::cout << "Dogs after scope: " << Counter<Dog>::instances << "\n";

    User user{"Alice", 30};
    Product product{"Widget", 9.99};
    user.print();
    product.print();

    return 0;
}
