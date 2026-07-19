#include "package_manager.hpp"

#include <iostream>

int main() {
    std::cout << "=== Package Manager Demo ===\n\n";

    package_manager::PackageManager pm;
    pm.addPackage({"fmt", "10.0", "https://github.com/fmtlib/fmt", {}});
    pm.addPackage({"spdlog", "1.12", "https://github.com/gabime/spdlog", {"fmt"}});
    pm.addPackage({"myapp", "1.0", "", {"spdlog"}});

    std::cout << "Dependencies of myapp:\n";
    for (const auto& dep : pm.resolveDependencies("myapp")) {
        std::cout << "  " << dep << "\n";
    }

    pm.install("fmt");
    pm.install("spdlog");
    std::cout << "\nInstalled: " << pm.installedCount() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
