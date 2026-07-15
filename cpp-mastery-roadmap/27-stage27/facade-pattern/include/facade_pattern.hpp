#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace facade_pattern {

class CPU {
public:
    std::string freeze() { return "CPU: freeze"; }
    std::string jump(std::size_t addr) { return "CPU: jump to " + std::to_string(addr); }
    std::string execute() { return "CPU: execute"; }
};

class Memory {
public:
    std::string load(std::size_t addr, const std::string& data) {
        return "Memory: load '" + data + "' at " + std::to_string(addr);
    }
};

class HardDrive {
public:
    std::string read(std::size_t sector, std::size_t size) {
        return "HD: read " + std::to_string(size) + " bytes from sector " + std::to_string(sector);
    }
};

class ComputerFacade {
public:
    ComputerFacade() : bootAddress_(0x00) {}

    std::vector<std::string> start() {
        std::vector<std::string> log;
        log.push_back(cpu_.freeze());
        log.push_back(hdd_.read(bootAddress_, 1024));
        log.push_back(mem_.load(bootAddress_, "bootloader"));
        log.push_back(cpu_.jump(bootAddress_));
        log.push_back(cpu_.execute());
        return log;
    }

    std::vector<std::string> shutdown() {
        std::vector<std::string> log;
        log.push_back("ComputerFacade: shutdown sequence");
        log.push_back(cpu_.freeze());
        return log;
    }

private:
    CPU cpu_;
    Memory mem_;
    HardDrive hdd_;
    std::size_t bootAddress_;
};

} // namespace facade_pattern
