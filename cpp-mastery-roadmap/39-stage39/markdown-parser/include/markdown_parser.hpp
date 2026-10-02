#pragma once

#include <string>
#include <vector>

namespace markdown_parser {

enum class BlockType { Heading, ListItem, Paragraph, CodeBlock, Quote };

struct Block {
    BlockType type;
    int level;          // heading level 1-6, list indent depth for items
    std::string text;   // stripped content
    std::string lang;   // code fence language ("" if none)
};

struct Link {
    std::string text;
    std::string url;
};

// Split markdown source into top-level blocks.
// Fenced code blocks are kept verbatim (no inner parsing).
std::vector<Block> parseBlocks(const std::string& source);

// Extract inline links from a single line: [text](url)
std::vector<Link> extractLinks(const std::string& line);

// Strip inline formatting markers: **bold**, *italic*, `code`
std::string stripInline(const std::string& text);

}  // namespace markdown_parser
