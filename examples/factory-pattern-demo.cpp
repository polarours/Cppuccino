// examples/factory-pattern-demo.cpp
// Demonstrates the Factory Method Pattern with auto-registration.
// Compile: g++ -std=c++20 -o factory-pattern-demo factory-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>

namespace factory_pattern {

class Product {
public:
    virtual ~Product() = default;
    virtual std::string name() const = 0;
    virtual double price() const = 0;
};

class Book : public Product {
public:
    Book(const std::string& title, double price)
        : title_(title), price_(price) {}
    std::string name() const override { return "Book: " + title_; }
    double price() const override { return price_; }
private:
    std::string title_;
    double price_;
};

class Electronics : public Product {
public:
    Electronics(const std::string& model, double price)
        : model_(model), price_(price) {}
    std::string name() const override { return "Electronics: " + model_; }
    double price() const override { return price_; }
private:
    std::string model_;
    double price_;
};

class Clothing : public Product {
public:
    Clothing(const std::string& item, double price)
        : item_(item), price_(price) {}
    std::string name() const override { return "Clothing: " + item_; }
    double price() const override { return price_; }
private:
    std::string item_;
    double price_;
};

class ProductFactory {
public:
    using Creator = std::function<std::unique_ptr<Product>(const std::string&, double)>;
    static void registerProduct(const std::string& type, Creator c) { getRegistry()[type] = std::move(c); }
    static std::unique_ptr<Product> create(const std::string& type, const std::string& name, double price) {
        auto& reg = getRegistry();
        auto it = reg.find(type);
        if (it == reg.end()) throw std::runtime_error("Unknown: " + type);
        return it->second(name, price);
    }
private:
    static std::map<std::string, Creator>& getRegistry() {
        static std::map<std::string, Creator> r;
        return r;
    }
};

struct BookReg   { BookReg()   { ProductFactory::registerProduct("book",        [](const std::string& n, double p){ return std::make_unique<Book>(n,p); }); }};
struct ElecReg   { ElecReg()   { ProductFactory::registerProduct("electronics", [](const std::string& n, double p){ return std::make_unique<Electronics>(n,p); }); }};
struct ClothReg  { ClothReg()  { ProductFactory::registerProduct("clothing",    [](const std::string& n, double p){ return std::make_unique<Clothing>(n,p); }); }};
static BookReg  _br;
static ElecReg  _er;
static ClothReg _cr;

} // namespace factory_pattern

int main() {
    using namespace factory_pattern;
    std::cout << "=== Factory Pattern Demo ===\n\n";

    auto b = ProductFactory::create("book", "C++ Primer", 49.99);
    std::cout << b->name() << " - $" << b->price() << "\n";

    auto e = ProductFactory::create("electronics", "Laptop", 999.99);
    std::cout << e->name() << " - $" << e->price() << "\n";

    auto c = ProductFactory::create("clothing", "T-Shirt", 19.99);
    std::cout << c->name() << " - $" << c->price() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
