#include "template_method.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Template Method Demo ===\n\n";

    // CSV miner
    std::cout << "CSV Mining:\n";
    template_method::CSVDataMiner csv;
    csv.mine("data.csv");

    // JSON miner with custom hook
    std::cout << "\nJSON Mining:\n";
    template_method::JSONDataMiner json;
    json.mine("data.json");

    // XML miner
    std::cout << "\nXML Mining:\n";
    template_method::XMLDataMiner xml;
    xml.mine("data.xml");

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
