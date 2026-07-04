#pragma once

#include "store/key_value_store.hpp"
#include "persistence/store_persistence.hpp"

#include <string>
#include <mutex>

namespace key_value_store {

class StoreManager {
public:
    explicit StoreManager(std::string persistencePath);

    bool load();
    bool save();

    KeyValueStore& store();
    const KeyValueStore& store() const;

    const std::string& persistencePath() const;
    void setPersistencePath(std::string path);

    void setAutoPersist(bool enabled);
    bool autoPersist() const;

private:
    mutable std::mutex persistenceMutex_;
    KeyValueStore store_;
    std::string persistencePath_;
    bool autoPersist_{true};
};

} // namespace key_value_store
