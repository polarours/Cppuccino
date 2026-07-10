#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace builder_pattern {

// --- Product ---

class Computer {
public:
    void setCPU(const std::string& cpu) { cpu_ = cpu; }
    void setRAM(const std::string& ram) { ram_ = ram; }
    void setStorage(const std::string& storage) { storage_ = storage; }
    void setGPU(const std::string& gpu) { gpu_ = gpu; }

    std::string getCPU() const { return cpu_; }
    std::string getRAM() const { return ram_; }
    std::string getStorage() const { return storage_; }
    std::string getGPU() const { return gpu_; }

    void display() const {
        std::cout << "Computer Configuration:\n";
        std::cout << "  CPU: " << cpu_ << "\n";
        std::cout << "  RAM: " << ram_ << "\n";
        std::cout << "  Storage: " << storage_ << "\n";
        std::cout << "  GPU: " << gpu_ << "\n";
    }

private:
    std::string cpu_;
    std::string ram_;
    std::string storage_;
    std::string gpu_;
};

// --- Builder Interface ---

class ComputerBuilder {
public:
    virtual ~ComputerBuilder() = default;

    virtual void buildCPU() = 0;
    virtual void buildRAM() = 0;
    virtual void buildStorage() = 0;
    virtual void buildGPU() = 0;

    std::shared_ptr<Computer> getResult() { return computer_; }

protected:
    std::shared_ptr<Computer> computer_ = std::make_shared<Computer>();
};

// --- Concrete Builders ---

class GamingComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override { computer_->setCPU("Intel i9-12900K"); }
    void buildRAM() override { computer_->setRAM("32GB DDR5"); }
    void buildStorage() override { computer_->setStorage("2TB NVMe SSD"); }
    void buildGPU() override { computer_->setGPU("NVIDIA RTX 4090"); }
};

class OfficeComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override { computer_->setCPU("Intel i5-12400"); }
    void buildRAM() override { computer_->setRAM("16GB DDR4"); }
    void buildStorage() override { computer_->setStorage("512GB SSD"); }
    void buildGPU() override { computer_->setGPU("Integrated Graphics"); }
};

class WorkstationBuilder : public ComputerBuilder {
public:
    void buildCPU() override { computer_->setCPU("AMD Threadripper"); }
    void buildRAM() override { computer_->setRAM("64GB DDR5"); }
    void buildStorage() override { computer_->setStorage("4TB NVMe RAID"); }
    void buildGPU() override { computer_->setGPU("NVIDIA A6000"); }
};

// --- Director ---

class ComputerDirector {
public:
    void setBuilder(std::shared_ptr<ComputerBuilder> builder) {
        builder_ = builder;
    }

    std::shared_ptr<Computer> construct() {
        builder_->buildCPU();
        builder_->buildRAM();
        builder_->buildStorage();
        builder_->buildGPU();
        return builder_->getResult();
    }

private:
    std::shared_ptr<ComputerBuilder> builder_;
};

} // namespace builder_pattern
