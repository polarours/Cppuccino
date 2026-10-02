#include "markdown_parser.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_headings() {
    auto blocks = markdown_parser::parseBlocks("# One\n### Three\n");
    expect(blocks.size() == 2, "expected 2 headings");
    expect(blocks[0].type == markdown_parser::BlockType::Heading, "block 0 is heading");
    expect(blocks[0].level == 1, "level 1");
    expect(blocks[0].text == "One", "text One");
    expect(blocks[1].level == 3, "level 3");
}

void test_list_items() {
    auto blocks = markdown_parser::parseBlocks("- alpha\n- beta\n");
    expect(blocks.size() == 2, "expected 2 items");
    expect(blocks[0].type == markdown_parser::BlockType::ListItem, "is list item");
    expect(blocks[0].text == "alpha", "alpha");
    expect(blocks[1].text == "beta", "beta");
}

void test_code_fence_not_parsed() {
    auto blocks = markdown_parser::parseBlocks("# H\n```\n# not a heading\n```\n");
    expect(blocks.size() == 2, "expected heading + code block");
    expect(blocks[1].type == markdown_parser::BlockType::CodeBlock, "code block");
}

void test_links() {
    auto links = markdown_parser::extractLinks("see [docs](guide.md) and [site](https://x.io)");
    expect(links.size() == 2, "expected 2 links");
    expect(links[0].text == "docs" && links[0].url == "guide.md", "first link");
    expect(links[1].text == "site" && links[1].url == "https://x.io", "second link");
}

void test_strip_inline() {
    expect(markdown_parser::stripInline("**bold**") == "bold", "bold");
    expect(markdown_parser::stripInline("`code` x") == "code x", "code");
    expect(markdown_parser::stripInline("*it*") == "it", "italic");
}

void test_paragraph_merge() {
    auto blocks = markdown_parser::parseBlocks("line one\nline two\n");
    expect(blocks.size() == 1, "merged into 1 paragraph");
    expect(blocks[0].text == "line one line two", "merged text");
}

}  // namespace

int main() {
    try {
        test_headings();
        test_list_items();
        test_code_fence_not_parsed();
        test_links();
        test_strip_inline();
        test_paragraph_merge();
        std::cout << "All markdown_parser tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
