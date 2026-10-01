// examples/balking-demo.cpp
// Demonstrates the Balking Pattern: an operation only proceeds when the
// object is in the right state - otherwise it bails out immediately.
// Compile: g++ -std=c++20 -Wall -Wextra -o balking-demo balking-demo.cpp

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

namespace balking_demo {

// A document that may be saved only while it is dirty.
class Document {
public:
    explicit Document(std::string name) : name_(std::move(name)) {}

    void edit(const std::string& content) {
        content_ = content;
        dirty_ = true;
        std::cout << "  edited: \"" << content_ << "\"\n";
    }

    // Balking: save() only does work when dirty_ == true.
    bool save() {
        std::lock_guard<std::mutex> lock(m_);
        if (!dirty_) {
            // Balk: nothing to do, state is not "dirty"
            std::cout << "  save() balked - \"" << name_ << "\" is clean\n";
            return false;
        }
        savedContent_ = content_;
        dirty_ = false;
        std::cout << "  saved \"" << name_ << "\" (" << savedContent_.size()
                  << " bytes)\n";
        return true;
    }

    bool dirty() const { return dirty_; }
    const std::string& savedContent() const { return savedContent_; }

private:
    std::string name_;
    std::string content_;
    std::string savedContent_;
    bool dirty_ = false;
    mutable std::mutex m_;
};

// Two threads race to save; exactly one should win the first time.
class AutoSaver {
public:
    explicit AutoSaver(Document& doc) : doc_(doc) {}

    void start() {
        for (int i = 0; i < 3; ++i) {
            std::thread([this, i] {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if (doc_.save()) {
                    ++wins_;
                    std::cout << "  [thread " << i << "] won the save\n";
                } else {
                    std::cout << "  [thread " << i << "] saw clean doc, bailed\n";
                }
            }).join();
        }
    }

    int wins() const { return wins_; }

private:
    Document& doc_;
    std::atomic<int> wins_{0};
};

} // namespace balking_demo

int main() {
    using namespace balking_demo;
    std::cout << "=== Balking Pattern Demo ===\n\n";

    Document doc("notes.txt");

    std::cout << "1. Save an untouched (clean) document:\n";
    doc.save();  // balks - never edited

    std::cout << "\n2. Edit then save:\n";
    doc.edit("hello balking");
    doc.save();

    std::cout << "\n3. Save again (already clean) - save() returns false:\n";
    bool third = doc.save();
    std::cout << "  third save returned " << std::boolalpha << third << "\n";

    std::cout << "\n4. Concurrent savers on a dirty doc:\n";
    Document race("race.txt");
    race.edit("payload");
    AutoSaver saver(race);
    saver.start();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
