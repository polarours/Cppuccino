#include "csv_parser.hpp"

#include <iostream>

int main() {
    const std::string text =
        "name,city,note\n"
        "alice,beijing,\"hello, world\"\n"
        "bob,\"new york\",plain\n";

    auto table = csv_parser::parse(text);
    std::cout << "columns:";
    for (auto& h : table.header) std::cout << " " << h;
    std::cout << "\nrows: " << table.rows.size() << "\n";
    for (auto& row : table.rows)
        std::cout << "  " << csv_parser::formatRow(row) << "\n";
    return 0;
}
