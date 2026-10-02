#include "text_diff.hpp"

#include <iostream>

int main() {
    const std::string old_doc = "line 1\nline 2\nline 3\n";
    const std::string new_doc = "line 1\nline two\nline 3\nline 4\n";

    auto hunks = text_diff::diffLines(text_diff::splitLines(old_doc),
                                      text_diff::splitLines(new_doc));
    std::cout << "--- diff ---\n" << text_diff::render(hunks);
    std::cout << "similarity: " << text_diff::similarity(hunks) << "\n";
    return 0;
}
