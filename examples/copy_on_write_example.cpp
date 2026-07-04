// examples/copy_on_write_example.cpp
// Demonstrates Copy-on-Write (COW) pattern in C++.
// Compile with: g++ -std=c++17 -o cow_example copy_on_write_example.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

// COW string implementation
class CowString {
public:
    CowString(const char* str = "") 
        : data_(std::make_shared<Data>(str)) {
        std::cout << "[Create] CowString: " << data_->content << " (use_count: " << data_.use_count() << ")\n";
    }
    
    CowString(const CowString& other) 
        : data_(other.data_) {
        std::cout << "[Share] CowString: " << data_->content << " (use_count: " << data_.use_count() << ")\n";
    }
    
    CowString& operator=(const CowString& other) {
        if (this != &other) {
            data_ = other.data_;
            std::cout << "[Share Assign] CowString: " << data_->content << " (use_count: " << data_.use_count() << ")\n";
        }
        return *this;
    }
    
    // Mutable access triggers copy
    char& operator[](size_t index) {
        detach();
        return data_->content[index];
    }
    
    // Const access doesn't trigger copy
    const char& operator[](size_t index) const {
        return data_->content[index];
    }
    
    size_t size() const { return data_->content.size(); }
    const char* c_str() const { return data_->content.c_str(); }
    
    void print() const {
        std::cout << "String: \"" << data_->content << "\" (use_count: " << data_.use_count() << ")\n";
    }

private:
    struct Data {
        std::string content;
        Data(const char* str) : content(str) {}
    };
    
    std::shared_ptr<Data> data_;
    
    void detach() {
        if (data_.use_count() > 1) {
            std::cout << "[Detach] Copying data for modification\n";
            data_ = std::make_shared<Data>(data_->content.c_str());
        }
    }
};

// COW vector implementation
template<typename T>
class CowVector {
public:
    CowVector() : data_(std::make_shared<Data>()) {}
    
    CowVector(std::initializer_list<T> init) 
        : data_(std::make_shared<Data>(init)) {}
    
    CowVector(const CowVector& other) 
        : data_(other.data_) {
        std::cout << "[Share] CowVector (use_count: " << data_.use_count() << ")\n";
    }
    
    CowVector& operator=(const CowVector& other) {
        if (this != &other) {
            data_ = other.data_;
        }
        return *this;
    }
    
    // Mutable access triggers copy
    T& operator[](size_t index) {
        detach();
        return data_->items[index];
    }
    
    // Const access doesn't trigger copy
    const T& operator[](size_t index) const {
        return data_->items[index];
    }
    
    void push_back(const T& value) {
        detach();
        data_->items.push_back(value);
    }
    
    size_t size() const { return data_->items.size(); }
    
    void print() const {
        std::cout << "Vector [";
        for (size_t i = 0; i < data_->items.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data_->items[i];
        }
        std::cout << "] (use_count: " << data_.use_count() << ")\n";
    }

private:
    struct Data {
        std::vector<T> items;
        Data() = default;
        Data(std::initializer_list<T> init) : items(init) {}
    };
    
    std::shared_ptr<Data> data_;
    
    void detach() {
        if (data_.use_count() > 1) {
            std::cout << "[Detach] Copying vector for modification\n";
            data_ = std::make_shared<Data>(data_->items);
        }
    }
};

// Demonstrate COW string
void demonstrateCowString() {
    std::cout << "=== COW String Demo ===\n";
    
    CowString s1("Hello");
    s1.print();
    
    CowString s2 = s1;  // Shared, no copy
    s2.print();
    
    CowString s3 = s1;  // Shared, no copy
    s3.print();
    
    std::cout << "\n--- Modifying s2 (triggers detach) ---\n";
    s2[0] = 'J';  // This copies the data
    s2.print();
    s1.print();
    s3.print();
    
    std::cout << "\n--- Modifying s3 (triggers detach) ---\n";
    s3[0] = 'W';  // This copies the data
    s3.print();
    s1.print();
}

// Demonstrate COW vector
void demonstrateCowVector() {
    std::cout << "\n=== COW Vector Demo ===\n";
    
    CowVector<int> v1 = {1, 2, 3, 4, 5};
    v1.print();
    
    CowVector<int> v2 = v1;  // Shared, no copy
    v2.print();
    
    CowVector<int> v3 = v1;  // Shared, no copy
    v3.print();
    
    std::cout << "\n--- Modifying v2 (triggers detach) ---\n";
    v2[0] = 100;  // This copies the data
    v2.print();
    v1.print();
    v3.print();
    
    std::cout << "\n--- Pushing to v3 (triggers detach) ---\n";
    v3.push_back(6);  // This copies the data
    v3.print();
    v1.print();
}

// Demonstrate benefits of COW
void demonstrateCOWBenefits() {
    std::cout << "\n=== COW Benefits Demo ===\n";
    
    // Creating many copies is cheap
    std::vector<CowString> strings;
    CowString original("Shared data");
    
    for (int i = 0; i < 5; ++i) {
        strings.push_back(original);  // All share the same data
    }
    
    std::cout << "\nAll strings share data:\n";
    for (const auto& s : strings) {
        s.print();
    }
    
    std::cout << "\n--- Modifying one string ---\n";
    strings[2][0] = 'X';  // Only this one copies
    for (const auto& s : strings) {
        s.print();
    }
}

int main() {
    demonstrateCowString();
    demonstrateCowVector();
    demonstrateCOWBenefits();
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
