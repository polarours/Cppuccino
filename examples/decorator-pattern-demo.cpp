// examples/decorator-pattern-demo.cpp
// Demonstrates Decorator Pattern: FileDataSource with Encryption + Compression + Logging.
// Compile: g++ -std=c++20 -o decorator-pattern-demo decorator-pattern-demo.cpp

#include <iostream>
#include <memory>
#include <string>

namespace decorator_pattern {

class DataSource {
public:
    virtual ~DataSource() = default;
    virtual void writeData(const std::string& s) = 0;
    virtual std::string readData() = 0;
};

class FileDataSource : public DataSource {
public:
    explicit FileDataSource(const std::string& fn) : filename_(fn) {}
    void writeData(const std::string& s) override { data_ = s; std::cout << "[File] Written to " << filename_ << ": " << s << "\n"; }
    std::string readData() override { return data_; }
private:
    std::string filename_;
    std::string data_;
};

class DataSourceDecorator : public DataSource {
public:
    explicit DataSourceDecorator(std::shared_ptr<DataSource> w) : wrapped_(std::move(w)) {}
    void writeData(const std::string& s) override { wrapped_->writeData(s); }
    std::string readData() override { return wrapped_->readData(); }
protected:
    std::shared_ptr<DataSource> wrapped_;
};

class EncryptionDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;
    void writeData(const std::string& s) override {
        std::cout << "[Encrypt] Encrypting\n";
        std::string e = s; for (auto& c : e) c = static_cast<char>(c + 1);
        DataSourceDecorator::writeData(e);
    }
    std::string readData() override {
        std::string d = DataSourceDecorator::readData();
        for (auto& c : d) c = static_cast<char>(c - 1);
        std::cout << "[Decrypt] Decrypting\n";
        return d;
    }
};

class CompressionDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;
    void writeData(const std::string& s) override {
        std::cout << "[Compress] Compressing\n";
        DataSourceDecorator::writeData("[compressed:" + s + "]");
    }
    std::string readData() override {
        std::string d = DataSourceDecorator::readData();
        if (d.find("[compressed:") == 0)
            d = d.substr(12, d.size() - 13);
        std::cout << "[Decompress] Decompressing\n";
        return d;
    }
};

class LoggingDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;
    void writeData(const std::string& s) override {
        std::cout << "[Log] Writing: " << s << "\n";
        DataSourceDecorator::writeData(s);
    }
    std::string readData() override {
        std::string d = DataSourceDecorator::readData();
        std::cout << "[Log] Reading: " << d << "\n";
        return d;
    }
};

} // namespace decorator_pattern

int main() {
    using namespace decorator_pattern;
    std::cout << "=== Decorator Pattern Demo ===\n\n";

    auto file = std::make_shared<FileDataSource>("data.txt");
    auto enc  = std::make_shared<EncryptionDecorator>(file);
    auto cmp  = std::make_shared<CompressionDecorator>(enc);
    auto log  = std::make_shared<LoggingDecorator>(cmp);

    log->writeData("Secret Message");
    std::cout << "\nRead back: " << log->readData() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
