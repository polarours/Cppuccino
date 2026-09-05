// examples/event-sourcing-demo.cpp
// Demonstrates Event Sourcing: EventStore, Account aggregate, replay from events.
// Compile: g++ -std=c++20 -o event-sourcing-demo event-sourcing-demo.cpp

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace event_sourcing {

struct Event {
    std::string type;
    std::string data;
    std::size_t version;
};

class EventStore {
public:
    void append(const Event& e) {
        Event ne = e;
        ne.version = nextVersion_++;
        events_.push_back(ne);
    }
    std::vector<Event> getEvents() const { return events_; }
    std::size_t version() const { return nextVersion_ - 1; }
    void clear() { events_.clear(); nextVersion_ = 1; }
private:
    std::vector<Event> events_;
    std::size_t nextVersion_ = 1;
};

class Account {
public:
    void applyEvent(const Event& e) {
        if (e.type == "Created")    { name_ = e.data; }
        else if (e.type == "Deposited")  { balance_ += std::stod(e.data); }
        else if (e.type == "Withdrawn")  { balance_ -= std::stod(e.data); }
    }
    const std::string& name()   const { return name_; }
    double              balance() const { return balance_; }
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
        std::ostringstream oss; oss << amount;
        Event e{"Deposited", oss.str(), 0};
        store_.append(e);
        account_.applyEvent(e);
    }
    void withdraw(double amount) {
        std::ostringstream oss; oss << amount;
        Event e{"Withdrawn", oss.str(), 0};
        store_.append(e);
        account_.applyEvent(e);
    }
    void rebuild() {
        account_ = Account();
        for (const auto& e : store_.getEvents()) account_.applyEvent(e);
    }
    const Account& account() const { return account_; }
    std::size_t eventCount() const { return store_.getEvents().size(); }
private:
    EventStore& store_;
    Account account_;
};

} // namespace event_sourcing

int main() {
    using namespace event_sourcing;
    std::cout << "=== Event Sourcing Demo ===\n\n";

    EventStore store;
    AccountAggregate agg(store);

    agg.create("Alice");
    agg.deposit(1000.0);
    agg.deposit(500.0);
    agg.withdraw(200.0);

    std::cout << "Current: " << agg.account().name()
              << " balance=$" << agg.account().balance()
              << " (events: " << agg.eventCount() << ")\n";

    // Simulate rebuild from event store
    AccountAggregate rebuilt(store);
    rebuilt.rebuild();
    std::cout << "Rebuilt: " << rebuilt.account().name()
              << " balance=$" << rebuilt.account().balance() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
