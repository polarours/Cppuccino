#include "package_manager.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_add_package() {
    package_manager::PackageManager pm;
    pm.addPackage({"fmt", "10.0", "https://github.com/fmtlib/fmt", {}});

    expect(pm.hasPackage("fmt"), "expected fmt");
    expect(pm.packageCount() == 1, "expected 1 package");
}

void test_dependencies() {
    package_manager::PackageManager pm;
    pm.addPackage({"A", "1.0", "", {}});
    pm.addPackage({"B", "1.0", "", {"A"}});
    pm.addPackage({"C", "1.0", "", {"B"}});

    auto deps = pm.resolveDependencies("C");
    expect(deps.size() == 3, "expected 3 deps");
    expect(deps[0] == "A", "expected A first");
    expect(deps[1] == "B", "expected B second");
    expect(deps[2] == "C", "expected C last");
}

void test_install() {
    package_manager::PackageManager pm;
    pm.addPackage({"fmt", "10.0", "", {}});

    expect(pm.install("fmt"), "expected install success");
    expect(pm.isInstalled("fmt"), "expected installed");
    expect(pm.installedCount() == 1, "expected 1 installed");
}

void test_install_missing() {
    package_manager::PackageManager pm;
    expect(!pm.install("missing"), "expected install failure");
}

void test_list_packages() {
    package_manager::PackageManager pm;
    pm.addPackage({"A", "1.0", "", {}});
    pm.addPackage({"B", "1.0", "", {}});

    auto list = pm.listPackages();
    expect(list.size() == 2, "expected 2 packages");
}

} // namespace

int main() {
    try {
        std::cout << "Package Manager Tests:\n";
        test_add_package();
        std::cout << "  test_add_package: PASS\n";

        test_dependencies();
        std::cout << "  test_dependencies: PASS\n";

        test_install();
        std::cout << "  test_install: PASS\n";

        test_install_missing();
        std::cout << "  test_install_missing: PASS\n";

        test_list_packages();
        std::cout << "  test_list_packages: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "package_manager_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\npackage_manager_tests passed" << std::endl;
    return 0;
}
