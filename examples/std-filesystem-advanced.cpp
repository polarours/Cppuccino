#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

// std::filesystem - filesystem operations (C++17)
// <filesystem>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== std::filesystem (C++17) ===\n\n";

    // Create directory
    fs::path demoDir = "demo_fs_test";
    if (!fs::exists(demoDir)) {
        fs::create_directory(demoDir);
        std::cout << "Created: " << demoDir << "\n";
    }

    // Create file
    fs::path filePath = demoDir / "test.txt";
    {
        std::ofstream file(filePath);
        file << "Hello, Filesystem!";
    }
    std::cout << "Created file: " << filePath << "\n";

    // File operations
    std::cout << "File exists: " << fs::exists(filePath) << "\n";
    std::cout << "File size: " << fs::file_size(filePath) << " bytes\n";
    std::cout << "Is regular: " << fs::is_regular_file(filePath) << "\n";

    // Read file
    std::ifstream inFile(filePath);
    std::string content;
    std::getline(inFile, content);
    std::cout << "Content: " << content << "\n";

    // Path operations
    fs::path p = "/home/user/documents/report.txt";
    std::cout << "\nPath operations:\n";
    std::cout << "  root: " << p.root_path() << "\n";
    std::cout << "  parent: " << p.parent_path() << "\n";
    std::cout << "  filename: " << p.filename() << "\n";
    std::cout << "  stem: " << p.stem() << "\n";
    std::cout << "  extension: " << p.extension() << "\n";

    // Directory iteration
    std::cout << "\nDirectory listing of " << demoDir << ":\n";
    for (const auto& entry : fs::directory_iterator(demoDir)) {
        std::cout << "  " << entry.path().filename()
                  << " (" << fs::file_size(entry) << " bytes)\n";
    }

    // Recursive directory iteration
    std::cout << "\nRecursive listing:\n";
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.path().string().find("demo_fs_test") != std::string::npos) {
            std::cout << "  " << entry.path() << "\n";
        }
    }

    // Rename
    fs::path newPath = demoDir / "renamed.txt";
    fs::rename(filePath, newPath);
    std::cout << "\nRenamed to: " << newPath << "\n";

    // Remove
    fs::remove(newPath);
    fs::remove(demoDir);
    std::cout << "Cleaned up\n";

    // Space info
    auto space = fs::space("/");
    std::cout << "\nDisk space (/):\n";
    std::cout << "  Total: " << space.capacity / (1024*1024*1024) << " GB\n";
    std::cout << "  Free: " << space.free / (1024*1024*1024) << " GB\n";

    return 0;
}
