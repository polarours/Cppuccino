#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace service_locator {

class ServiceLocator {
public:
    void registerService(const std::string& name, std::shared_ptr<void> service) {
        services_[name] = std::move(service);
    }

    std::shared_ptr<void> getRaw(const std::string& name) const {
        auto it = services_.find(name);
        return (it != services_.end()) ? it->second : nullptr;
    }

    template <typename T>
    std::shared_ptr<T> getService(const std::string& name) const {
        return std::static_pointer_cast<T>(getRaw(name));
    }

    bool hasService(const std::string& name) const {
        return services_.find(name) != services_.end();
    }

    void removeService(const std::string& name) {
        services_.erase(name);
    }

    std::size_t serviceCount() const { return services_.size(); }

private:
    std::map<std::string, std::shared_ptr<void>> services_;
};

} // namespace service_locator
