#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace decorator_pattern {

// --- Component Interface ---

class DataSource {
public:
    virtual ~DataSource() = default;
    virtual void writeData(const std::string& data) = 0;
    virtual std::string readData() = 0;
};

// --- Concrete Component ---

class FileDataSource : public DataSource {
public:
    explicit FileDataSource(const std::string& filename)
        : filename_(filename) {}

    void writeData(const std::string& data) override {
        std::cout << "[File] Writing to " << filename_ << ": " << data << "\n";
    }

    std::string readData() override {
        return "Data from " + filename_;
    }

private:
    std::string filename_;
};

// --- Decorator Base ---

class DataSourceDecorator : public DataSource {
public:
    explicit DataSourceDecorator(std::shared_ptr<DataSource> wrapped)
        : wrapped_(std::move(wrapped)) {}

    void writeData(const std::string& data) override {
        wrapped_->writeData(data);
    }

    std::string readData() override {
        return wrapped_->readData();
    }

protected:
    std::shared_ptr<DataSource> wrapped_;
};

// --- Concrete Decorators ---

class EncryptionDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;

    void writeData(const std::string& data) override {
        std::cout << "[Encrypt] Encrypting data\n";
        DataSourceDecorator::writeData(encrypt(data));
    }

    std::string readData() override {
        std::string data = DataSourceDecorator::readData();
        std::cout << "[Decrypt] Decrypting data\n";
        return decrypt(data);
    }

private:
    std::string encrypt(const std::string& data) {
        std::string result = data;
        for (auto& c : result) c += 1;
        return result;
    }

    std::string decrypt(const std::string& data) {
        std::string result = data;
        for (auto& c : result) c -= 1;
        return result;
    }
};

class CompressionDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;

    void writeData(const std::string& data) override {
        std::cout << "[Compress] Compressing data\n";
        DataSourceDecorator::writeData(compress(data));
    }

    std::string readData() override {
        std::string data = DataSourceDecorator::readData();
        std::cout << "[Decompress] Decompressing data\n";
        return decompress(data);
    }

private:
    std::string compress(const std::string& data) {
        return "[compressed:" + data + "]";
    }

    std::string decompress(const std::string& data) {
        if (data.substr(0, 12) == "[compressed:") {
            return data.substr(12, data.size() - 13);
        }
        return data;
    }
};

class LoggingDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;

    void writeData(const std::string& data) override {
        std::cout << "[Log] Writing: " << data << "\n";
        DataSourceDecorator::writeData(data);
    }

    std::string readData() override {
        std::string data = DataSourceDecorator::readData();
        std::cout << "[Log] Read: " << data << "\n";
        return data;
    }
};

} // namespace decorator_pattern
