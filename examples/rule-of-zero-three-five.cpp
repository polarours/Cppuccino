// examples/rule_of_zero_three_five.cpp
// Demonstrates Rule of Zero, Three, and Five in C++.
// Compile with: g++ -std=c++17 -o rule_of_3_5 rule_of_zero_three_five.cpp

#include <iostream>
#include <string>
#include <memory>
#include <vector>

// Rule of Zero: classes that manage no resources
class SimplePoint {
public:
    SimplePoint(int x, int y) : x_(x), y_(y) {}
    
    int x() const { return x_; }
    int y() const { return y_; }
    
    void print() const {
        std::cout << "(" << x_ << ", " << y_ << ")\n";
    }

private:
    int x_;
    int y_;
    // No special member functions needed - compiler generates correct ones
};

// Rule of Three: classes that manage resources (raw pointer)
class RawBuffer {
public:
    explicit RawBuffer(size_t size) : size_(size), data_(new int[size]) {
        std::cout << "[Acquire] Allocated buffer of size " << size_ << '\n';
    }
    
    ~RawBuffer() {
        delete[] data_;
        std::cout << "[Release] Freed buffer of size " << size_ << '\n';
    }
    
    // Copy constructor
    RawBuffer(const RawBuffer& other) : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "[Copy] Copied buffer of size " << size_ << '\n';
    }
    
    // Copy assignment operator
    RawBuffer& operator=(const RawBuffer& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new int[size_];
            std::copy(other.data_, other.data_ + size_, data_);
            std::cout << "[Copy Assign] Copied buffer of size " << size_ << '\n';
        }
        return *this;
    }
    
    // Move constructor
    RawBuffer(RawBuffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;
        std::cout << "[Move] Moved buffer of size " << size_ << '\n';
    }
    
    // Move assignment operator
    RawBuffer& operator=(RawBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
            std::cout << "[Move Assign] Moved buffer of size " << size_ << '\n';
        }
        return *this;
    }
    
    int& operator[](size_t index) { return data_[index]; }
    const int& operator[](size_t index) const { return data_[index]; }
    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

// Rule of Five: classes with move semantics (using smart pointers)
class ModernBuffer {
public:
    explicit ModernBuffer(size_t size) : size_(size), data_(std::make_unique<int[]>(size)) {
        std::cout << "[Acquire] Allocated buffer of size " << size_ << '\n';
    }
    
    // No destructor needed - unique_ptr handles cleanup
    // No copy constructor/assignment - deleted by default (unique_ptr is non-copyable)
    // Move constructor and assignment auto-generated correctly
    
    int& operator[](size_t index) { return data_[index]; }
    const int& operator[](size_t index) const { return data_[index]; }
    size_t size() const { return size_; }
    
    // Explicit copy if needed
    ModernBuffer copy() const {
        ModernBuffer newBuffer(size_);
        std::copy(data_.get(), data_.get() + size_, newBuffer.data_.get());
        return newBuffer;
    }

private:
    size_t size_;
    std::unique_ptr<int[]> data_;
};

// Rule of Zero with composition
class Logger {
public:
    Logger(const std::string& name) : name_(name) {
        std::cout << "[Logger] Created: " << name_ << '\n';
    }
    
    ~Logger() {
        std::cout << "[Logger] Destroyed: " << name_ << '\n';
    }
    
    void log(const std::string& message) const {
        std::cout << "[" << name_ << "] " << message << '\n';
    }

private:
    std::string name_;
};

class Application {
public:
    Application(const std::string& appName) : name_(appName) {
        std::cout << "[App] Created: " << name_ << '\n';
    }
    
    // No special member functions needed - all members handle themselves
    
    void run() {
        logger_.log("Application started");
    }

private:
    std::string name_;
    Logger logger_;
};

int main() {
    std::cout << "=== Rule of Zero Demo ===\n";
    SimplePoint p1(10, 20);
    SimplePoint p2 = p1;  // Copy works correctly
    SimplePoint p3 = std::move(p1);  // Move works correctly
    p2.print();
    p3.print();
    
    std::cout << "\n=== Rule of Three Demo ===\n";
    {
        RawBuffer buf1(5);
        for (size_t i = 0; i < buf1.size(); ++i) {
            buf1[i] = static_cast<int>(i * 10);
        }
        
        RawBuffer buf2 = buf1;  // Copy construction
        RawBuffer buf3(3);
        buf3 = buf1;  // Copy assignment
        
        RawBuffer buf4 = std::move(buf1);  // Move construction
        
        std::cout << "buf2[2] = " << buf2[2] << '\n';
        std::cout << "buf3[3] = " << buf3[3] << '\n';
    }  // All buffers destroyed here
    
    std::cout << "\n=== Rule of Five Demo ===\n";
    {
        ModernBuffer buf1(5);
        for (size_t i = 0; i < buf1.size(); ++i) {
            buf1[i] = static_cast<int>(i * 10);
        }
        
        ModernBuffer buf2 = buf1.copy();  // Explicit copy
        ModernBuffer buf3 = std::move(buf1);  // Move (automatic)
        
        std::cout << "buf2[2] = " << buf2[2] << '\n';
        std::cout << "buf3[3] = " << buf3[3] << '\n';
    }  // All buffers destroyed here
    
    std::cout << "\n=== Rule of Zero with Composition Demo ===\n";
    {
        Application app("MyApp");
        app.run();
    }
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
