// examples/proxy-demo.cpp
// Demonstrates the Proxy Pattern: a surrogate controlling access to a
// real object (lazy loading + access logging).
// Compile: g++ -std=c++20 -Wall -Wextra -o proxy-demo proxy-demo.cpp

#include <iostream>
#include <memory>
#include <string>

namespace proxy_demo {

// Subject interface
class Image {
public:
    virtual ~Image() = default;
    virtual void display() = 0;
};

// Real subject: expensive to create (would read pixels from disk)
class RealImage : public Image {
public:
    explicit RealImage(std::string filename) : filename_(std::move(filename)) {
        loadFromDisk();  // costly
    }
    void display() override {
        std::cout << "Displaying " << filename_ << "\n";
    }
private:
    void loadFromDisk() {
        std::cout << "[loading " << filename_ << " from disk]\n";
    }
    std::string filename_;
};

// Proxy: defers creation until first display, logs every access
class ImageProxy : public Image {
public:
    explicit ImageProxy(std::string filename) : filename_(std::move(filename)) {}

    void display() override {
        ++accesses_;
        if (!real_) {
            real_ = std::make_unique<RealImage>(filename_);
        }
        std::cout << "[proxy: access #" << accesses_ << "]\n";
        real_->display();
    }

    int accesses() const { return accesses_; }

private:
    std::string filename_;
    std::unique_ptr<RealImage> real_;
    int accesses_ = 0;
};

} // namespace proxy_demo

int main() {
    using namespace proxy_demo;
    std::cout << "=== Proxy Pattern Demo ===\n\n";

    // Creating the proxy is cheap - no disk load yet
    ImageProxy photo("cat.png");
    std::cout << "Proxy constructed (no load yet)\n\n";

    std::cout << "-- first display --\n";
    photo.display();   // triggers loading

    std::cout << "\n-- second display --\n";
    photo.display();   // reuses loaded image

    std::cout << "\nTotal accesses: " << photo.accesses() << "\n";
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
