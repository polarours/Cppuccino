// examples/std-filesystem.cpp
// Demonstrates C++17 filesystem library.
// Compile with: g++ -std=c++17 -lstdc++fs -o std_filesystem std-filesystem.cpp

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

// --- 1. Path operations ---

void pathOperations() {
    std::cout << "--- Path Operations ---\n";

    fs::path p = "/home/user/documents/file.txt";

    std::cout << "Path: " << p << "\n";
    std::cout << "Root: " << p.root_path() << "\n";
    std::cout << "Parent: " << p.parent_path() << "\n";
    std::cout << "Filename: " << p.filename() << "\n";
    std::cout << "Stem: " << p.stem() << "\n";
    std::cout << "Extension: " << p.extension() << "\n";
}

// --- 2. Path manipulation ---

void pathManipulation() {
    std::cout << "\n--- Path Manipulation ---\n";

    fs::path p = "/home/user";

    p /= "documents";
    std::cout << "After /= : " << p << "\n";

    p /= "file.txt";
    std::cout << "After /= : " << p << "\n";

    p = p.replace_extension(".md");
    std::cout << "Replace extension: " << p << "\n";

    p = p.replace_filename("readme");
    std::cout << "Replace filename: " << p << "\n";
}

// --- 3. File status ---

void fileStatus() {
    std::cout << "\n--- File Status ---\n";

    fs::path p = ".";

    if (fs::exists(p)) {
        std::cout << "Exists: yes\n";
        std::cout << "Is directory: " << fs::is_directory(p) << "\n";
        std::cout << "Is file: " << fs::is_regular_file(p) << "\n";
    }
}

// --- 4. Directory iteration ---

void directoryIteration() {
    std::cout << "\n--- Directory Iteration ---\n";

    fs::path dir = ".";

    std::cout << "Contents of current directory:\n";
    for (const auto& entry : fs::directory_iterator(dir)) {
        std::cout << "  " << entry.path().filename();
        if (fs::is_directory(entry)) {
            std::cout << "/";
        }
        std::cout << "\n";
    }
}

// --- 5. Recursive directory iteration ---

void recursiveIteration() {
    std::cout << "\n--- Recursive Directory Iteration ---\n";

    fs::path dir = ".";

    std::cout << "Files in examples/:\n";
    int count = 0;
    for (const auto& entry : fs::recursive_directory_iterator(dir / "examples")) {
        if (fs::is_regular_file(entry)) {
            std::cout << "  " << entry.path().filename() << "\n";
            count++;
            if (count >= 5) {
                std::cout << "  ... (showing first 5)\n";
                break;
            }
        }
    }
}

// --- 6. Create and remove ---

void createRemove() {
    std::cout << "\n--- Create and Remove ---\n";

    fs::path testDir = "test_dir";
    fs::path testFile = testDir / "test.txt";

    // Create directory
    fs::create_directories(testDir);
    std::cout << "Created directory: " << testDir << "\n";

    // Create file
    {
        std::ofstream file(testFile);
        file << "Hello, filesystem!";
    }
    std::cout << "Created file: " << testFile << "\n";

    // Check exists
    std::cout << "File exists: " << fs::exists(testFile) << "\n";

    // Remove
    fs::remove(testFile);
    fs::remove(testDir);
    std::cout << "Removed file and directory\n";
}

// --- 7. Copy and rename ---

void copyRename() {
    std::cout << "\n--- Copy and Rename ---\n";

    fs::path source = "test_source.txt";
    fs::path dest = "test_dest.txt";

    // Create source file
    {
        std::ofstream file(source);
        file << "test content";
    }

    // Copy
    fs::copy_file(source, dest, fs::copy_options::overwrite_existing);
    std::cout << "Copied " << source << " to " << dest << "\n";

    // Rename
    fs::path renamed = "test_renamed.txt";
    fs::rename(dest, renamed);
    std::cout << "Renamed to " << renamed << "\n";

    // Cleanup
    fs::remove(source);
    fs::remove(renamed);
}

// --- 8. File size and time ---

void fileSizeTime() {
    std::cout << "\n--- File Size and Time ---\n";

    fs::path p = ".";

    if (fs::exists(p)) {
        auto size = fs::file_size(p);
        std::cout << "Size: " << size << " bytes\n";

        auto lastWrite = fs::last_write_time(p);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWrite - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        auto time = std::chrono::system_clock::to_time_t(sctp);
        std::cout << "Last write: " << std::ctime(&time);
    }
}

// --- 9. Canonical path ---

void canonicalPath() {
    std::cout << "\n--- Canonical Path ---\n";

    fs::path p = "../current_dir/.";

    if (fs::exists(p)) {
        fs::path canonical = fs::canonical(p);
        std::cout << "Canonical: " << canonical << "\n";
    }
}

// --- 10. Space info ---

void spaceInfo() {
    std::cout << "\n--- Space Info ---\n";

    fs::space_info space = fs::space("/");
    std::cout << "Capacity: " << space.capacity / (1024 * 1024 * 1024) << " GB\n";
    std::cout << "Free: " << space.free / (1024 * 1024 * 1024) << " GB\n";
    std::cout << "Available: " << space.available / (1024 * 1024 * 1024) << " GB\n";
}

int main() {
    pathOperations();
    pathManipulation();
    fileStatus();
    directoryIteration();
    recursiveIteration();
    createRemove();
    copyRename();
    fileSizeTime();
    canonicalPath();
    spaceInfo();

    std::cout << "\n=== All filesystem scenarios completed ===\n";
    return 0;
}
