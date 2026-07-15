#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace unit_of_work {

struct Entity {
    int id;
    std::string data;
};

enum class ChangeType { None, Add, Modify, Remove };

struct TrackedEntity {
    Entity entity;
    ChangeType changeType = ChangeType::None;
};

class UnitOfWork {
public:
    void registerNew(const Entity& entity) {
        tracked_[entity.id] = {entity, ChangeType::Add};
    }

    void registerModified(const Entity& entity) {
        auto it = tracked_.find(entity.id);
        if (it != tracked_.end() && it->second.changeType == ChangeType::Add) {
            it->second.entity = entity;
        } else {
            tracked_[entity.id] = {entity, ChangeType::Modify};
        }
    }

    void registerRemoved(int id) {
        auto it = tracked_.find(id);
        if (it != tracked_.end() && it->second.changeType == ChangeType::Add) {
            tracked_.erase(it);
        } else {
            Entity dummy;
            dummy.id = id;
            tracked_[id] = {dummy, ChangeType::Remove};
        }
    }

    void commit() {
        for (auto& [id, tracked] : tracked_) {
            committed_.push_back(tracked);
        }
        tracked_.clear();
    }

    void rollback() {
        tracked_.clear();
    }

    bool hasChanges() const { return !tracked_.empty(); }
    std::size_t pendingCount() const { return tracked_.size(); }
    const std::vector<TrackedEntity>& committed() const { return committed_; }

private:
    std::map<int, TrackedEntity> tracked_;
    std::vector<TrackedEntity> committed_;
};

} // namespace unit_of_work
