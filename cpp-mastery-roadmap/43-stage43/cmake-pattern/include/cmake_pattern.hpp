#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace cmake_pattern {

struct BuildOption {
    std::string name;
    std::string description;
    std::string defaultValue;
    bool enabled = false;
};

class CMakeProject {
public:
    void setName(const std::string& name) { name_ = name; }
    void setVersion(const std::string& version) { version_ = version; }
    void setCxxStandard(int standard) { cxxStandard_ = standard; }

    void addOption(const std::string& name, const std::string& desc, bool def = false) {
        options_[name] = {name, desc, def ? "ON" : "OFF", def};
    }

    void addLibrary(const std::string& name) { libraries_.push_back(name); }
    void addExecutable(const std::string& name) { executables_.push_back(name); }
    void addTest(const std::string& name) { tests_.push_back(name); }

    void setDefine(const std::string& key, const std::string& value) {
        defines_[key] = value;
    }

    std::string generate() const {
        std::string out;
        out += "cmake_minimum_required(VERSION 3.16)\n\n";
        out += "project(" + name_ + " VERSION " + version_ + " LANGUAGES CXX)\n\n";
        out += "set(CMAKE_CXX_STANDARD " + std::to_string(cxxStandard_) + ")\n";
        out += "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";

        for (const auto& [key, val] : options_) {
            out += "option(" + val.name + " \"" + val.description + "\" " + val.defaultValue + ")\n";
        }
        if (!options_.empty()) out += "\n";

        for (const auto& lib : libraries_) {
            out += "add_library(" + lib + " src/" + lib + ".cpp)\n";
            out += "target_include_directories(" + lib + " PUBLIC include)\n";
        }
        if (!libraries_.empty()) out += "\n";

        for (const auto& exe : executables_) {
            out += "add_executable(" + exe + " src/" + exe + ".cpp)\n";
        }
        if (!executables_.empty()) out += "\n";

        out += "enable_testing()\n\n";

        for (const auto& test : tests_) {
            out += "add_executable(" + test + " tests/" + test + ".cpp)\n";
            out += "target_link_libraries(" + test + " PRIVATE " + name_ + "_core)\n";
            out += "add_test(NAME " + test + " COMMAND " + test + ")\n";
        }

        return out;
    }

    const std::string& name() const { return name_; }
    const std::string& version() const { return version_; }
    int cxxStandard() const { return cxxStandard_; }
    std::size_t optionCount() const { return options_.size(); }
    std::size_t libraryCount() const { return libraries_.size(); }
    std::size_t testCount() const { return tests_.size(); }

private:
    std::string name_ = "MyProject";
    std::string version_ = "1.0.0";
    int cxxStandard_ = 17;
    std::map<std::string, BuildOption> options_;
    std::vector<std::string> libraries_;
    std::vector<std::string> executables_;
    std::vector<std::string> tests_;
    std::map<std::string, std::string> defines_;
};

} // namespace cmake_pattern
