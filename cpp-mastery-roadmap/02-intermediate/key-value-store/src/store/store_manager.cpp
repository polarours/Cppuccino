#include "store_manager.hpp"

namespace key_value_store {

StoreManager::StoreManager(std::string persistencePath)
    : persistencePath_(std::move(persistencePath)) {}

bool StoreManager::load() {
    std::lock_guard lock(persistenceMutex_);
    try {
        store_.replace_all(StorePersistence::load(persistencePath_));
        return true;
    } catch (...) {
        return false;
    }
}

bool StoreManager::save() {
    std::lock_guard lock(persistenceMutex_);
    try {
        StorePersistence::save(persistencePath_, store_.snapshot());
        return true;
    } catch (...) {
        return false;
    }
}

KeyValueStore& StoreManager::store() {
    return store_;
}

const KeyValueStore& StoreManager::store() const {
    return store_;
}

const std::string& StoreManager::persistencePath() const {
    return persistencePath_;
}

void StoreManager::setPersistencePath(std::string path) {
    persistencePath_ = std::move(path);
}

void StoreManager::setAutoPersist(bool enabled) {
    autoPersist_ = enabled;
}

bool StoreManager::autoPersist() const {
    return autoPersist_;
}

} // namespace key_value_store
