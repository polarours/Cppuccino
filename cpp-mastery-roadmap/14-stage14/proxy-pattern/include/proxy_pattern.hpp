#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace proxy_pattern {

// --- Subject Interface ---

class Image {
public:
    virtual ~Image() = default;
    virtual void display() const = 0;
    virtual std::string getInfo() const = 0;
};

// --- Real Subject ---

class RealImage : public Image {
public:
    explicit RealImage(const std::string& filename) : filename_(filename) {
        loadFromDisk();
    }

    void display() const override {
        std::cout << "[RealImage] Displaying: " << filename_ << "\n";
    }

    std::string getInfo() const override {
        return "RealImage: " + filename_;
    }

private:
    void loadFromDisk() {
        std::cout << "[RealImage] Loading from disk: " << filename_ << "\n";
    }

    std::string filename_;
};

// --- Proxy ---

class ImageProxy : public Image {
public:
    explicit ImageProxy(const std::string& filename)
        : filename_(filename) {}

    void display() const override {
        if (!realImage_) {
            std::cout << "[Proxy] Creating real image on first access\n";
            realImage_ = std::make_unique<RealImage>(filename_);
        }
        realImage_->display();
    }

    std::string getInfo() const override {
        return "ImageProxy: " + filename_ +
               (realImage_ ? " (loaded)" : " (not loaded)");
    }

private:
    std::string filename_;
    mutable std::unique_ptr<RealImage> realImage_;
};

} // namespace proxy_pattern
