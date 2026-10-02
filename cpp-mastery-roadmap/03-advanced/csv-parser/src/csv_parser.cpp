#include "csv_parser.hpp"

#include <stdexcept>

namespace csv_parser {

namespace {

bool needsQuoting(const std::string& f) {
    if (f.find_first_of(",\"\n\r") != std::string::npos) return true;
    return false;
}

std::string quote(const std::string& f) {
    std::string out = "\"";
    for (char c : f) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += "\"";
    return out;
}

// Parse fields of one row starting at pos; stops at newline (consumed).
std::vector<std::string> parseRow(const std::string& text, std::size_t& pos) {
    std::vector<std::string> row;
    std::string field;
    bool inQuotes = false;
    while (pos < text.size()) {
        char c = text[pos];
        if (inQuotes) {
            if (c == '"') {
                if (pos + 1 < text.size() && text[pos + 1] == '"') {
                    field += '"';
                    pos += 2;
                } else {
                    inQuotes = false;
                    ++pos;
                }
            } else {
                field += c;
                ++pos;
            }
            continue;
        }
        if (c == '"' && field.empty()) {
            inQuotes = true;
            ++pos;
        } else if (c == ',') {
            row.push_back(field);
            field.clear();
            ++pos;
        } else if (c == '\n' || c == '\r') {
            if (c == '\r' && pos + 1 < text.size() && text[pos + 1] == '\n') ++pos;
            ++pos;
            row.push_back(field);
            return row;
        } else {
            field += c;
            ++pos;
        }
    }
    row.push_back(field);  // last row without trailing newline
    return row;
}

}  // namespace

CsvTable parse(const std::string& text) {
    CsvTable table;
    if (text.empty()) return table;
    std::size_t pos = 0;
    table.header = parseRow(text, pos);
    while (pos < text.size()) {
        auto row = parseRow(text, pos);
        // skip a single trailing empty line
        if (row.size() == 1 && row[0].empty() && pos >= text.size()) break;
        table.rows.push_back(std::move(row));
    }
    return table;
}

std::string formatRow(const std::vector<std::string>& fields) {
    std::string out;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (i) out += ',';
        out += needsQuoting(fields[i]) ? quote(fields[i]) : fields[i];
    }
    return out;
}

std::size_t CsvTable::columnIndex(const std::string& name) const {
    for (std::size_t i = 0; i < header.size(); ++i)
        if (header[i] == name) return i;
    return header.size();  // npos-like
}

std::vector<std::string> CsvTable::rowAsMap(
    const std::vector<std::string>& row) const {
    // "map" as aligned vector: missing columns become ""
    std::vector<std::string> out(header.size());
    for (std::size_t i = 0; i < header.size(); ++i)
        out[i] = i < row.size() ? row[i] : "";
    return out;
}

}  // namespace csv_parser
