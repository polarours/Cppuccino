#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <format>

// C++20 std::filesystem - cross-platform file operations
// Demonstrates file system traversal and manipulation

namespace fs = std::filesystem;

void printDirectory(const fs::path& dir, int indent = 0) {
    std::string indent_str(indent * 2, ' ');
    
    for (const auto& entry : fs::directory_iterator(dir)) {
        std::cout << indent_str << entry.path().filename().string();
        
        if (entry.is_directory()) {
            std::cout << "/";
        } else if (entry.is_regular_file()) {
            std::cout << " (" << entry.file_size() << " bytes)";
        }
        std::cout << "\n";
        
        if (entry.is_directory() && entry.path().filename() != "." && 
            entry.path().filename() != "..") {
            printDirectory(entry.path(), indent + 1);
        }
    }
}

int main() {
    std::cout << "=== C++20 std::filesystem Demo ===\n\n";

    // Current path
    std::cout << "1. Current path:\n";
    auto current = fs::current_path();
    std::cout << "   " << current.string() << "\n";

    // Path operations
    std::cout << "\n2. Path operations:\n";
    fs::path p = "/home/user/documents/file.txt";
    std::cout << "   Parent: " << p.parent_path().string() << "\n";
    std::cout << "   Filename: " << p.filename().string() << "\n";
    std::cout << "   Extension: " << p.extension().string() << "\n";
    std::cout << "   Stem: " << p.stem().string() << "\n";

    // Path concatenation
    std::cout << "\n3. Path concatenation:\n";
    fs::path base = "/home/user";
    fs::path child = base / "documents";
    std::cout << "   " << child.string() << "\n";

    // File system query
    std::cout << "\n4. File system queries:\n";
    if (fs::exists(current)) {
        std::cout << "   Current path exists: yes\n";
        std::cout << "   Is directory: " << std::boolalpha << fs::is_directory(current) << "\n";
        std::cout << "   Is regular file: " << fs::is_regular_file(current) << "\n";
        std::cout << "   Is symlink: " << fs::is_symlink(current) << "\n";
    }

    // Directory iterator
    std::cout << "\n5. Directory contents (./examples):\n";
    fs::path examples = fs::current_path() / "examples";
    if (fs::exists(examples)) {
        std::cout << "   Files in examples:\n";
        for (const auto& entry : fs::directory_iterator(examples)) {
            std::cout << "   - " << entry.path().filename().string() 
                      << " (" << entry.file_size() << " bytes)\n";
        }
    }

    // File operations
    std::cout << "\n6. File operations:\n";
    fs::path test_file = fs::current_path() / "test_output.txt";
    
    // Write file
    {
        std::ofstream out(test_file);
        out << "Hello, C++20 filesystem!\n";
    }
    std::cout << "   Created: " << test_file.string() << "\n";
    std::cout << "   Size: " << fs::file_size(test_file) << " bytes\n";

    // Rename
    fs::path renamed = fs::current_path() / "renamed_output.txt";
    fs::rename(test_file, renamed);
    std::cout << "   Renamed to: " << renamed.filename().string() << "\n";

    // Copy
    fs::path copied = fs::current_path() / "copy_output.txt";
    fs::copy(renamed, copied, fs::copy_options::skip_existing);
    std::cout << "   Copied to: " << copied.filename().string() << "\n";

    // Remove
    fs::remove(copied);
    fs::remove(renamed);
    std::cout << "   Cleaned up test files\n";

    // Temporary path
    std::cout << "\n7. Temporary path:\n";
    std::cout << "   Temp directory: " << fs::temp_directory_path().string() << "\n";

    // Recursive directory iterator
    std::cout << "\n8. Recursive search (find .cpp files):\n";
    int count = 0;
    for (const auto& entry : fs::recursive_directory_iterator(fs::current_path() / "examples")) {
        if (entry.path().extension() == ".cpp") {
            std::cout << "   " << entry.path().filename().string() << "\n";
            if (++count >= 5) break;  // Limit output
        }
    }

    return 0;
}
