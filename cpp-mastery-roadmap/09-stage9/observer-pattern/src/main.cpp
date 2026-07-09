#include "observer_pattern.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== Observer Pattern Demo ===\n\n";

    observer_pattern::Subject<std::string> subject;

    auto logger = std::make_shared<observer_pattern::Logger<std::string>>();
    auto counter = std::make_shared<observer_pattern::Counter<std::string>>();

    subject.attach(logger);
    subject.attach(counter);

    subject.setValue("first");
    subject.setValue("second");
    subject.setValue("third");

    std::cout << "\nTotal updates: " << counter->getCount() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
