// examples/iterator-pattern-demo.cpp
// Demonstrates the iterator design pattern: an iterator interface, aggregates that create one, and the client code that never knows the difference.
// Compile with: g++ -std=c++17 -o iterator_pattern_demo iterator-pattern-demo.cpp

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

// --- 1. The iterator interface ---

class Iterator {
public:
    virtual ~Iterator() = default;
    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual std::string currentItem() const = 0;
};

// --- 2. The aggregate interface ---

class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual Iterator* createIterator() const = 0;
};

// --- 3. The concrete iterators ---

// An iterator over a list.
class ListIterator : public Iterator {
public:
    explicit ListIterator(const std::list<std::string>& list)
        : list_(list), current_(list_.begin()) {}

    void first() override { current_ = list_.begin(); }

    void next() override { ++current_; }

    bool isDone() const override { return current_ == list_.end(); }

    std::string currentItem() const override { return *current_; }

private:
    const std::list<std::string>& list_;
    std::list<std::string>::const_iterator current_;
};

// An iterator over a vector.
class VectorIterator : public Iterator {
public:
    explicit VectorIterator(const std::vector<int>& numbers)
        : numbers_(numbers), index_(0) {}

    void first() override { index_ = 0; }

    void next() override { ++index_; }

    bool isDone() const override { return index_ >= numbers_.size(); }

    std::string currentItem() const override {
        return std::to_string(numbers_[index_]);
    }

private:
    const std::vector<int>& numbers_;
    std::size_t index_ = 0;
};

// An iterator over the values of a map.
class MapIterator : public Iterator {
public:
    explicit MapIterator(const std::map<std::string, int>& scores)
        : scores_(scores), current_(scores_.begin()) {}

    void first() override { current_ = scores_.begin(); }

    void next() override { ++current_; }

    bool isDone() const override { return current_ == scores_.end(); }

    std::string currentItem() const override {
        return current_->first + "=" + std::to_string(current_->second);
    }

private:
    const std::map<std::string, int>& scores_;
    std::map<std::string, int>::const_iterator current_;
};

// --- 4. The concrete aggregates ---

class BookShelf : public Aggregate {
public:
    void add(const std::string& title) { books_.push_back(title); }

    Iterator* createIterator() const override {
        return new ListIterator(books_);
    }

private:
    std::list<std::string> books_;
};

class NumberSeries : public Aggregate {
public:
    void add(int number) { numbers_.push_back(number); }

    Iterator* createIterator() const override {
        return new VectorIterator(numbers_);
    }

private:
    std::vector<int> numbers_;
};

class ScoreTable : public Aggregate {
public:
    void set(const std::string& name, int score) { scores_[name] = score; }

    Iterator* createIterator() const override {
        return new MapIterator(scores_);
    }

private:
    std::map<std::string, int> scores_;
};

// --- 5. The client ---

// The client walks any aggregate. It does not know what is behind it, and
// it does not have to change when a new aggregate appears.
void printAll(const Aggregate& aggregate) {
    std::unique_ptr<Iterator> iterator(aggregate.createIterator());
    for (iterator->first(); !iterator->isDone(); iterator->next()) {
        std::cout << "  " << iterator->currentItem() << "\n";
    }
}

int main() {
    std::cout << "=== Iterator Pattern Demo ===\n\n";

    BookShelf shelf;
    shelf.add("The C++ Programming Language");
    shelf.add("Effective Modern C++");
    shelf.add("The Standard C++ Library");

    std::cout << "The shelf:\n";
    printAll(shelf);

    NumberSeries series;
    series.add(3);
    series.add(1);
    series.add(4);

    std::cout << "\nThe series:\n";
    printAll(series);

    ScoreTable scores;
    scores.set("Alice", 91);
    scores.set("Bob", 78);

    std::cout << "\nThe scores:\n";
    printAll(scores);

    // The same client walks all three, because none of them leaks into it.
    std::cout << "\n=== The client never asked what it was walking ===\n";
    return 0;
}
