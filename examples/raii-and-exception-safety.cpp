// examples/raii_and_exception_safety.cpp
// Demonstrates RAII pattern and exception safety in C++.
// Compile with: g++ -std=c++17 -o raii_example raii_and_exception_safety.cpp

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

// RAII wrapper for a file handle
class FileGuard {
public:
    explicit FileGuard(const std::string& filename) 
        : file_(filename), filename_(filename) {
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::cout << "[Acquire] Opened file: " << filename_ << '\n';
    }
    
    ~FileGuard() {
        if (file_.is_open()) {
            file_.close();
            std::cout << "[Release] Closed file: " << filename_ << '\n';
        }
    }
    
    // Non-copyable, movable
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;
    
    FileGuard(FileGuard&& other) noexcept 
        : file_(std::move(other.file_)), filename_(std::move(other.filename_)) {
        std::cout << "[Move] FileGuard moved\n";
    }
    
    void write(const std::string& data) {
        file_ << data;
    }
    
    std::string filename() const { return filename_; }

private:
    std::fstream file_;
    std::string filename_;
};

// RAII wrapper for dynamic memory
template<typename T>
class ScopedPtr {
public:
    explicit ScopedPtr(T* ptr = nullptr) : ptr_(ptr) {}
    
    ~ScopedPtr() {
        delete ptr_;
        std::cout << "[Release] Deleted object\n";
    }
    
    ScopedPtr(const ScopedPtr&) = delete;
    ScopedPtr& operator=(const ScopedPtr&) = delete;
    
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    T* get() const { return ptr_; }
    
    T* release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

private:
    T* ptr_;
};

// Exception safety demonstration
void exceptionSafeOperation() {
    std::cout << "\n=== Exception Safety Demo ===\n";
    
    auto vec = std::make_unique<std::vector<int>>();
    
    try {
        // This will throw, but vec is automatically cleaned up
        vec->push_back(1);
        vec->push_back(2);
        vec->push_back(3);
        
        // Simulate an operation that might throw
        if (vec->size() > 2) {
            throw std::runtime_error("Size exceeded threshold");
        }
        
        vec->push_back(4);  // Never reached
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << '\n';
        std::cout << "Vector size at exception: " << vec->size() << '\n';
        // vec is automatically cleaned up here
    }
}

// RAII with file operations
void fileOperationDemo() {
    std::cout << "\n=== File RAII Demo ===\n";
    
    try {
        FileGuard writer("raii_demo.txt");
        writer.write("Line 1: Hello RAII\n");
        writer.write("Line 2: Exception safety\n");
        
        // Even if an exception occurs, file is closed automatically
        // throw std::runtime_error("Simulated error");
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << '\n';
    }
    // FileGuard destructor called here, file is closed
}

// Custom unique_ptr demonstration
void scopedPtrDemo() {
    std::cout << "\n=== ScopedPtr Demo ===\n";
    
    ScopedPtr<int> ptr(new int(42));
    std::cout << "Value: " << *ptr << '\n';
    
    ScopedPtr<std::string> strPtr(new std::string("RAII is powerful"));
    std::cout << "String: " << *strPtr << '\n';
    
    // Transfer ownership
    int* rawPtr = ptr.release();
    std::cout << "Released value: " << *rawPtr << '\n';
    delete rawPtr;
}

int main() {
    try {
        exceptionSafeOperation();
        fileOperationDemo();
        scopedPtrDemo();
        
        std::cout << "\n=== All demos completed successfully ===\n";
    } catch (const std::exception& e) {
        std::cout << "Unhandled exception: " << e.what() << '\n';
        return 1;
    }
    
    return 0;
}
