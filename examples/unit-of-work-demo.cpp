// examples/unit-of-work-demo.cpp
// Demonstrates the Unit of Work Pattern: track all changes to a set of
// objects and apply them as one atomic transaction.
// Compile: g++ -std=c++20 -Wall -Wextra -o unit-of-work-demo unit-of-work-demo.cpp

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace unit_of_work {

struct Account {
    int id;
    std::string owner;
    double balance;
};

// In-memory "database"
class Database {
public:
    static Database& instance() {
        static Database db;
        return db;
    }

    void insert(const Account& a) { rows_[a.id] = a; }
    void update(const Account& a) { rows_[a.id] = a; }
    void remove(int id) { rows_.erase(id); }

    void print() const {
        for (const auto& [id, a] : rows_)
            std::cout << "  id=" << id << " owner=" << a.owner
                      << " balance=" << a.balance << "\n";
    }

private:
    std::map<int, Account> rows_;
};

// Unit of Work: collect new/changed/deleted objects, commit in one shot
class UnitOfWork {
public:
    void registerNew(std::shared_ptr<Account> a) { newObjs_[a->id] = a; }
    void registerDirty(std::shared_ptr<Account> a) { dirtyObjs_[a->id] = a; }
    void registerRemoved(std::shared_ptr<Account> a) { removedObjs_[a->id] = a; }

    // Apply everything in a defined order; new+dirty wins over removed
    void commit() {
        auto& db = Database::instance();
        for (const auto& [id, obj] : newObjs_) {
            db.insert(*obj);
            std::cout << "  COMMIT insert id=" << id << "\n";
        }
        for (const auto& [id, obj] : dirtyObjs_) {
            if (newObjs_.count(id)) continue;
            db.update(*obj);
            std::cout << "  COMMIT update id=" << id << "\n";
        }
        for (const auto& [id, obj] : removedObjs_) {
            db.remove(id);
            std::cout << "  COMMIT delete id=" << id << "\n";
        }
        newObjs_.clear();
        dirtyObjs_.clear();
        removedObjs_.clear();
    }

    void rollback() {
        newObjs_.clear();
        dirtyObjs_.clear();
        removedObjs_.clear();
        std::cout << "  ROLLBACK (all pending changes discarded)\n";
    }

private:
    std::map<int, std::shared_ptr<Account>> newObjs_;
    std::map<int, std::shared_ptr<Account>> dirtyObjs_;
    std::map<int, std::shared_ptr<Account>> removedObjs_;
};

} // namespace unit_of_work

int main() {
    using namespace unit_of_work;
    std::cout << "=== Unit of Work Demo ===\n\n";

    Database::instance().insert({1, "alice", 1000.0});
    Database::instance().insert({2, "bob", 500.0});
    std::cout << "Initial state:\n";
    Database::instance().print();

    UnitOfWork uow;

    // Stage several changes across different objects
    auto alice = std::make_shared<Account>(Account{1, "alice", 1500.0});
    auto carol = std::make_shared<Account>(Account{3, "carol", 200.0});
    auto bob = std::make_shared<Account>(Account{2, "bob", 500.0});

    uow.registerDirty(alice);   // balance change
    uow.registerNew(carol);     // new account
    uow.registerRemoved(bob);   // close bob's account

    std::cout << "\nBefore commit (DB unchanged):\n";
    Database::instance().print();

    std::cout << "\nCommitting:\n";
    uow.commit();

    std::cout << "\nAfter commit:\n";
    Database::instance().print();

    // Rollback discards everything without touching the DB
    std::cout << "\nStaging a change then rolling back:\n";
    auto ghost = std::make_shared<Account>(Account{9, "ghost", 0.0});
    uow.registerNew(ghost);
    uow.rollback();
    std::cout << "DB after rollback (no ghost):\n";
    Database::instance().print();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
