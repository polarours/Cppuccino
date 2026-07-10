#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>

namespace factory_pattern {

// --- Product Interface ---

class Product {
public:
    virtual ~Product() = default;
    virtual std::string name() const = 0;
    virtual double price() const = 0;
};

// --- Concrete Products ---

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

// --- Factory ---

class ProductFactory {
public:
    using Creator = std::function<std::unique_ptr<Product>(const std::string& name, double price)>;

    static void registerProduct(const std::string& type, Creator creator) {
        getRegistry()[type] = std::move(creator);
    }

    static std::unique_ptr<Product> create(const std::string& type, const std::string& name, double price) {
        auto& registry = getRegistry();
        auto it = registry.find(type);
        if (it == registry.end()) {
            throw std::runtime_error("Unknown product type: " + type);
        }
        return it->second(name, price);
    }

private:
    static std::map<std::string, Creator>& getRegistry() {
        static std::map<std::string, Creator> registry;
        return registry;
    }
};

// --- Auto Registration ---

struct BookRegistrar {
    BookRegistrar() {
        ProductFactory::registerProduct("book", [](const std::string& name, double price) {
            return std::make_unique<Book>(name, price);
        });
    }
};

struct ElectronicsRegistrar {
    ElectronicsRegistrar() {
        ProductFactory::registerProduct("electronics", [](const std::string& name, double price) {
            return std::make_unique<Electronics>(name, price);
        });
    }
};

struct ClothingRegistrar {
    ClothingRegistrar() {
        ProductFactory::registerProduct("clothing", [](const std::string& name, double price) {
            return std::make_unique<Clothing>(name, price);
        });
    }
};

// Static instances for auto-registration
static BookRegistrar bookRegistrar;
static ElectronicsRegistrar electronicsRegistrar;
static ClothingRegistrar clothingRegistrar;

} // namespace factory_pattern
