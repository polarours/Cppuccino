#pragma once

#include <string>
#include <iostream>
#include <vector>

namespace template_method {

// --- Abstract Class ---

class DataMiner {
public:
    virtual ~DataMiner() = default;

    // Template method
    void mine(const std::string& path) {
        openFile(path);
        extractData();
        parseData();
        analyzeData();
        closeFile();
    }

    // Hook methods (can be overridden)
    virtual void openFile(const std::string& path) {
        std::cout << "[DataMiner] Opening file: " << path << "\n";
    }

    virtual void closeFile() {
        std::cout << "[DataMiner] Closing file\n";
    }

    // Abstract methods (must be overridden)
    virtual void extractData() = 0;
    virtual void parseData() = 0;
    virtual void analyzeData() = 0;
};

// --- Concrete Classes ---

class CSVDataMiner : public DataMiner {
public:
    void extractData() override {
        std::cout << "[CSV] Extracting CSV data\n";
    }

    void parseData() override {
        std::cout << "[CSV] Parsing CSV format\n";
    }

    void analyzeData() override {
        std::cout << "[CSV] Analyzing CSV data\n";
    }
};

class JSONDataMiner : public DataMiner {
public:
    void extractData() override {
        std::cout << "[JSON] Extracting JSON data\n";
    }

    void parseData() override {
        std::cout << "[JSON] Parsing JSON format\n";
    }

    void analyzeData() override {
        std::cout << "[JSON] Analyzing JSON data\n";
    }

    // Override hook
    void openFile(const std::string& path) override {
        std::cout << "[JSON] Opening JSON file: " << path << "\n";
    }
};

class XMLDataMiner : public DataMiner {
public:
    void extractData() override {
        std::cout << "[XML] Extracting XML data\n";
    }

    void parseData() override {
        std::cout << "[XML] Parsing XML format\n";
    }

    void analyzeData() override {
        std::cout << "[XML] Analyzing XML data\n";
    }
};

} // namespace template_method
