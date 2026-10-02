#include "markdown_parser.hpp"

#include <sstream>

namespace markdown_parser {

namespace {

bool startsWith(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    auto b = s.find_first_not_of(ws);
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

}  // namespace

std::vector<Block> parseBlocks(const std::string& source) {
    std::vector<Block> blocks;
    std::istringstream in(source);
    std::string line;
    bool inCode = false;
    Block para{BlockType::Paragraph, 0, "", ""};

    auto flushPara = [&]() {
        para.text = trim(para.text);
        if (!para.text.empty()) blocks.push_back(para);
        para = {BlockType::Paragraph, 0, "", ""};
    };

    while (std::getline(in, line)) {
        if (startsWith(line, "```")) {
            if (inCode) {
                blocks.push_back({BlockType::CodeBlock, 0, "", para.lang});
                para = {BlockType::Paragraph, 0, "", ""};
                inCode = false;
            } else {
                flushPara();
                inCode = true;
                para = {BlockType::CodeBlock, 0, "", trim(line.substr(3))};
            }
            continue;
        }
        if (inCode) {
            if (!para.text.empty()) para.text += "\n";
            para.text += line;
            continue;
        }
        if (!line.empty() && line[0] == '#') {
            flushPara();
            int level = 0;
            while (level < (int)line.size() && line[level] == '#') ++level;
            blocks.push_back({BlockType::Heading, level, trim(line.substr(level)), ""});
        } else if (startsWith(trim(line), "- ") || startsWith(trim(line), "* ")) {
            flushPara();
            std::string item = trim(line.substr(0, 1) + trim(line).substr(1));
            int depth = 0;
            std::string t = trim(line);
            blocks.push_back({BlockType::ListItem, depth, trim(t.substr(2)), ""});
        } else if (startsWith(trim(line), "> ")) {
            flushPara();
            blocks.push_back({BlockType::Quote, 0, trim(line).substr(2), ""});
        } else if (trim(line).empty()) {
            flushPara();
        } else {
            if (!para.text.empty()) para.text += " ";
            para.text += trim(line);
        }
    }
    if (inCode) {
        // Unclosed fence: emit what we have
        Block b{BlockType::CodeBlock, 0, para.text, para.lang};
        blocks.push_back(b);
    } else {
        flushPara();
    }
    return blocks;
}

std::vector<Link> extractLinks(const std::string& line) {
    std::vector<Link> links;
    std::size_t pos = 0;
    while ((pos = line.find(']', pos)) != std::string::npos) {
        std::size_t open = line.rfind('[', pos);
        std::size_t paren = line.find("](", pos);
        if (open == std::string::npos || paren != pos) { ++pos; continue; }
        std::size_t close = line.find(')', paren + 2);
        if (close == std::string::npos) break;
        links.push_back({line.substr(open + 1, pos - open - 1),
                         line.substr(paren + 2, close - paren - 2)});
        pos = close + 1;
    }
    return links;
}

std::string stripInline(const std::string& text) {
    // Drop the marker characters but KEEP the content between them:
    // "**bold**" -> "bold", "`code`" -> "code".
    std::string out;
    out.reserve(text.size());
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '*' || text[i] == '`') {
            char marker = text[i];
            std::size_t end = text.find(marker, i + 1);
            if (end != std::string::npos) {
                out.append(text, i + 1, end - i - 1);  // inner content
                i = end;
                continue;
            }
        }
        out.push_back(text[i]);
    }
    return out;
}

}  // namespace markdown_parser
