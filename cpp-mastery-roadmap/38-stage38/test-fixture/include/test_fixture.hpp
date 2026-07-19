#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>

namespace test_fixture {

struct TodoItem {
    int id;
    std::string text;
    bool done;
};

class TodoList {
public:
    int add(const std::string& text) {
        int id = nextId_++;
        items_.push_back({id, text, false});
        return id;
    }

    void complete(int id) {
        for (auto& item : items_) {
            if (item.id == id) {
                item.done = true;
                return;
            }
        }
    }

    void remove(int id) {
        items_.erase(
            std::remove_if(items_.begin(), items_.end(),
                [id](const TodoItem& i) { return i.id == id; }),
            items_.end());
    }

    std::vector<TodoItem> all() const { return items_; }

    std::size_t size() const { return items_.size(); }

    std::size_t completedCount() const {
        return std::count_if(items_.begin(), items_.end(),
            [](const TodoItem& i) { return i.done; });
    }

private:
    std::vector<TodoItem> items_;
    int nextId_ = 1;
};

class TodoTestFixture {
public:
    TodoTestFixture() {
        setup();
    }

    void setup() {
        list_ = TodoList();
        id1_ = list_.add("Buy milk");
        id2_ = list_.add("Walk dog");
        id3_ = list_.add("Write code");
    }

    void teardown() {
        list_ = TodoList();
    }

    TodoList& list() { return list_; }
    int id1() const { return id1_; }
    int id2() const { return id2_; }
    int id3() const { return id3_; }

private:
    TodoList list_;
    int id1_ = 0, id2_ = 0, id3_ = 0;
};

} // namespace test_fixture
