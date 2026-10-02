#pragma once

#include <string>
#include <vector>

namespace csv_parser {

struct CsvTable {
    std::vector<std::string> header;
    std::vector<std::vector<std::string>> rows;
    // Row lookup by column name ("" when the column is missing).
    std::vector<std::string> rowAsMap(const std::vector<std::string>& row) const;
    std::size_t columnIndex(const std::string& name) const;
};

// Parse RFC-4180-ish CSV:
//  - fields separated by ','
//  - quoted fields may contain commas, newlines and doubled quotes ("")
//  - first row is the header
// Throws std::runtime_error on malformed input (unterminated quote).
CsvTable parse(const std::string& text);

// Join fields into one CSV line (quotes when needed).
std::string formatRow(const std::vector<std::string>& fields);

}  // namespace csv_parser
