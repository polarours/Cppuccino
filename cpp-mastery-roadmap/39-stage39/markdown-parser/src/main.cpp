#include "markdown_parser.hpp"

#include <iostream>

int main() {
    const std::string doc =
        "# Title\n\nSome **bold** text with [a link](https://example.com).\n\n"
        "- first item\n- second item\n\n> quoted line\n\n"
        "```cpp\nint x = 1;\n```\n";

    auto blocks = markdown_parser::parseBlocks(doc);
    for (const auto& b : blocks) {
        switch (b.type) {
            case markdown_parser::BlockType::Heading:
                std::cout << "H" << b.level << ": " << b.text << "\n"; break;
            case markdown_parser::BlockType::ListItem:
                std::cout << "  - " << b.text << "\n"; break;
            case markdown_parser::BlockType::Quote:
                std::cout << "  > " << b.text << "\n"; break;
            case markdown_parser::BlockType::CodeBlock:
                std::cout << "CODE(" << b.lang << ")\n"; break;
            default:
                std::cout << "P: " << markdown_parser::stripInline(b.text) << "\n";
        }
    }
    auto links = markdown_parser::extractLinks(doc);
    for (const auto& l : links)
        std::cout << "LINK: [" << l.text << "] -> " << l.url << "\n";
    return 0;
}
