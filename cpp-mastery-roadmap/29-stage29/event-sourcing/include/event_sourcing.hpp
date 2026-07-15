#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>

namespace event_sourcing {

struct Event {
    std::string type;
    std::string data;
    std::size_t version;
};

class EventStore {
public:
    void append(const Event& event) {
        Event e = event;
        e.version = nextVersion_++;
        events_.push_back(e);
    }

    std::vector<Event> getEvents() const { return events_; }

    std::vector<Event> getEventsSince(std::size_t version) const {
        std::vector<Event> result;
        for (const auto& e : events_) {
            if (e.version >= version) result.push_back(e);
        }
        return result;
    }

    std::size_t version() const { return nextVersion_ - 1; }
    std::size_t size() const { return events_.size(); }
    void clear() { events_.clear(); nextVersion_ = 1; }

private:
    std::vector<Event> events_;
    std::size_t nextVersion_ = 1;
};

class Account {
public:
    void applyEvent(const Event& event) {
        if (event.type == "Created") {
            name_ = event.data;
        } else if (event.type == "Deposited") {
            balance_ += std::stod(event.data);
        } else if (event.type == "Withdrawn") {
            balance_ -= std::stod(event.data);
        }
    }

    const std::string& name() const { return name_; }
    double balance() const { return balance_; }

private:
    std::string name_;
    double balance_ = 0.0;
};

class AccountAggregate {
public:
    explicit AccountAggregate(EventStore& store) : store_(store) {}

    void create(const std::string& name) {
        store_.append({"Created", name, 0});
        account_.applyEvent({"Created", name, 0});
    }

    void deposit(double amount) {
        std::ostringstream oss;
        oss << amount;
        Event e{"Deposited", oss.str(), 0};
        store_.append(e);
        account_.applyEvent(e);
    }

    void withdraw(double amount) {
        std::ostringstream oss;
        oss << amount;
        Event e{"Withdrawn", oss.str(), 0};
        store_.append(e);
        account_.applyEvent(e);
    }

    void rebuild() {
        account_ = Account();
        for (const auto& event : store_.getEvents()) {
            account_.applyEvent(event);
        }
    }

    const Account& account() const { return account_; }

private:
    EventStore& store_;
    Account account_;
};

} // namespace event_sourcing
