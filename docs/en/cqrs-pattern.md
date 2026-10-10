# CQRS Pattern

## What Is the CQRS Pattern?

CQRS (Command Query Responsibility Segregation) splits modification
(commands) and reading (queries) onto separate models and channels, instead
of making one data model serve both jobs.

> **Core idea**: the write model exists only to keep state changes correct,
> the read model only to make queries fast — the two evolve independently
> and never drag each other down.

## How It Relates to Event Sourcing / CRUD

| Aspect | CRUD | Event Sourcing | CQRS |
|--------|------|----------------|------|
| Number of models | One model for both | One event stream (reads replay it) | Two models: read and write |
| Writes | Update current state directly | Append events | Commands go through the write model |
| Queries | Query tables directly | Replay, then query | Query the read-model projection |
| Consistency | Strong | Usually eventual | Strong on write, may lag on read |
| Role | Baseline | "How the write side is stored" | "How read/write are split" |

The three are orthogonal: event sourcing is about the **storage format**, CQRS
about the **read/write split**, and CRUD is the unsplit control group. CQRS is
often paired with event sourcing (the event store is the write model,
projections are the read models), but CQRS does not require it — plain tables
work fine.

## Core Concepts

- **Command**: an operation that changes state, with a definite outcome
- **Query**: read-only, changes nothing, safe to repeat at any time
- **Command Bus**: the dispatch channel for commands — the write-side entry point
- **Query Bus**: the dispatch channel for queries — the read-side entry point
- **Read model / write model**: the two may have completely different shapes
  (normalized vs denormalized)

## Problem Scenario and Solution

**Problem**: one model serves both reads and writes. Complex report queries are
forced to aggregate live on the write model — slow, and contending with writes.
Denormalizing for the query side would pollute the write model's normalized
structure.

```cpp
#include <string>
#include <vector>

// Anti-pattern: one model doing both jobs
struct Order {
    int id;
    double amount;
};

struct OrderStore {
    std::vector<Order> orders;  // write model (normalized)

    // complex query forced to aggregate on the write model
    std::string salesReport() {
        double total = 0;
        for (const auto& o : orders) total += o.amount;
        return "total:" + std::to_string(total);
    }
};
```

**Solution**: the write side maintains normalized state and updates a read
projection on every write; queries hit the projection, decoupled from the write
model's shape.

```cpp
// Good: write side owns state, read side owns the projection
#include <string>
#include <vector>

struct Order {
    int id;
    double amount;
};

struct WriteSide {
    std::vector<Order> orders;
    double total = 0;  // read projection, maintained on write

    void place(int id, double amount) {
        orders.push_back({id, amount});
        total += amount;
    }
};

struct ReadSide {
    const double& total;  // O(1) query, independent of write-model shape
};

int main() {
    WriteSide write;
    ReadSide read{write.total};
    write.place(1, 100.0);
    write.place(2, 50.0);
    return read.total == 150.0 ? 0 : 1;
}
```

## Implementation

The implementation below comes from the
[`cqrs-pattern` project](../../cpp-mastery-roadmap/28-stage28/cqrs-pattern/):
commands and queries travel on separate buses — write methods are invoked by
command handlers, read methods by query handlers.

```cpp
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace cqrs_pattern {

struct Account {
    int id;
    std::string name;
    double balance;
};

class CommandBus {
public:
    using Handler = std::function<void(const std::string&)>;

    void registerHandler(const std::string& commandType, Handler handler) {
        handlers_[commandType] = std::move(handler);
    }

    void dispatch(const std::string& commandType, const std::string& payload) {
        auto it = handlers_.find(commandType);
        if (it != handlers_.end()) {
            it->second(payload);
        }
    }

private:
    std::map<std::string, Handler> handlers_;
};

class QueryBus {
public:
    using Handler = std::function<std::string(const std::string&)>;

    void registerHandler(const std::string& queryType, Handler handler) {
        handlers_[queryType] = std::move(handler);
    }

    std::string query(const std::string& queryType, const std::string& params) {
        auto it = handlers_.find(queryType);
        if (it != handlers_.end()) {
            return it->second(params);
        }
        return "";
    }

private:
    std::map<std::string, Handler> handlers_;
};

class AccountService {
public:
    void createAccount(const std::string& name) {
        accounts_[nextId_] = {nextId_, name, 0.0};
        nextId_++;
    }

    void deposit(int id, double amount) {
        auto it = accounts_.find(id);
        if (it != accounts_.end()) {
            it->second.balance += amount;
        }
    }

    std::shared_ptr<Account> getAccount(int id) {
        auto it = accounts_.find(id);
        return (it != accounts_.end()) ? std::make_shared<Account>(it->second) : nullptr;
    }

    AccountService() {
        commands_.registerHandler("create", [this](const std::string& payload) {
            createAccount(payload);
        });
        commands_.registerHandler("deposit", [this](const std::string& payload) {
            auto pos = payload.find(':');
            int id = std::stoi(payload.substr(0, pos));
            double amount = std::stod(payload.substr(pos + 1));
            deposit(id, amount);
        });

        queries_.registerHandler("get", [this](const std::string& params) {
            int id = std::stoi(params);
            auto acc = getAccount(id);
            if (acc) return acc->name + ":" + std::to_string(acc->balance);
            return std::string();
        });
        queries_.registerHandler("list", [this](const std::string&) {
            std::string result;
            for (const auto& [id, acc] : accounts_) {
                if (!result.empty()) result += ",";
                result += acc.name;
            }
            return result;
        });
    }

    CommandBus& commands() { return commands_; }
    QueryBus& queries() { return queries_; }

private:
    std::map<int, Account> accounts_;
    int nextId_ = 1;
    CommandBus commands_;
    QueryBus queries_;
};

} // namespace cqrs_pattern

#include <cassert>
int main() {
    cqrs_pattern::AccountService svc;
    // write side: command channel
    svc.commands().dispatch("create", "alice");
    svc.commands().dispatch("create", "bob");
    svc.commands().dispatch("deposit", "1:100");
    // read side: query channel
    assert(svc.queries().query("get", "1") == "alice:100.000000");
    assert(svc.queries().query("get", "2") == "bob:0.000000");
    assert(svc.queries().query("list", "") == "alice,bob");
    // queries are repeatable and side-effect free
    assert(svc.queries().query("get", "1") == "alice:100.000000");
    return 0;
}
```

## Best Practices

1. **Check whether you actually need CQRS**: it pays off when read/write ratios
   diverge, the two sides scale differently, or they must evolve independently —
   in an ordinary CRUD app it only adds complexity
2. **Accept read-side lag**: projections may update asynchronously, but be
   prepared for a brief "wrote it, can't read it yet" window, or provide a
   consistency fallback
3. **Commands express intent**: name them with business verbs (`PlaceOrder`,
   not `Save`); one command does one thing and reports failure explicitly
4. **Queries have no side effects**: query handlers never mutate state, so they
   can be cached, retried, and parallelized freely
5. **When paired with event sourcing**: the event store is the sole write model
   and read models are projections of events — projections can be dropped and
   rebuilt at any time

## Summary

CQRS separates commands and queries onto different models and channels so the
write side can focus on consistency and the read side on query performance; it
composes with (but does not depend on) event sourcing, and should be adopted
only when the read/write difference is real.
