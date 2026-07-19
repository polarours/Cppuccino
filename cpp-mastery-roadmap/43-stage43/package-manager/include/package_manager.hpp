#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

namespace package_manager {

struct Package {
    std::string name;
    std::string version;
    std::string url;
    std::vector<std::string> dependencies;
};

class PackageManager {
public:
    void addPackage(const Package& pkg) {
        packages_[pkg.name] = pkg;
    }

    bool hasPackage(const std::string& name) const {
        return packages_.find(name) != packages_.end();
    }

    const Package& getPackage(const std::string& name) const {
        return packages_.at(name);
    }

    std::vector<std::string> resolveDependencies(const std::string& name) const {
        std::vector<std::string> result;
        std::map<std::string, bool> visited;

        auto resolve = [&](auto& self, const std::string& pkg) -> void {
            if (visited[pkg]) return;
            visited[pkg] = true;

            auto it = packages_.find(pkg);
            if (it == packages_.end()) return;

            for (const auto& dep : it->second.dependencies) {
                self(self, dep);
            }
            result.push_back(pkg);
        };

        resolve(resolve, name);
        return result;
    }

    bool install(const std::string& name) {
        if (!hasPackage(name)) return false;
        installed_.insert(name);
        return true;
    }

    bool isInstalled(const std::string& name) const {
        return installed_.find(name) != installed_.end();
    }

    std::size_t packageCount() const { return packages_.size(); }
    std::size_t installedCount() const { return installed_.size(); }

    std::vector<std::string> listPackages() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : packages_) {
            names.push_back(name);
        }
        return names;
    }

private:
    std::map<std::string, Package> packages_;
    std::set<std::string> installed_;
};

} // namespace package_manager
