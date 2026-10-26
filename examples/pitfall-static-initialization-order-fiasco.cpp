// examples/pitfall-static-initialization-order-fiasco.cpp
// Demonstrates the static initialization order fiasco and the fixes: Meyers' singleton, constexpr and constinit.
// Compile with: g++ -std=c++17 -o pitfall_static_initialization_order pitfall-static-initialization-order-fiasco.cpp

#include <iostream>
#include <string>

// --- 1. Two translation units, emulated with two namespaces ---
//
// logger.cpp would hold the logger and config.cpp the config. The standard
// leaves the dynamic initialization order across translation units
// unspecified, so config.cpp may run its initializer before logger.cpp does.

namespace tu_a {  // emulates logger.cpp

// A namespace scope bool with a constant initializer is constant initialized
// before any dynamic initialization runs, so reading it is safe even while
// the logger below is still unconstructed.
bool logger_ready = false;

struct Logger {
    Logger() {
        logger_ready = true;
        std::cout << "[init] tu_a::Logger constructed\n";
    }

    ~Logger() {
        logger_ready = false;
        std::cout << "[shutdown] tu_a::Logger destroyed\n";
    }

    void log(const std::string& message) const {
        std::cout << "[logger] " << message << "\n";
    }
};

// A namespace scope object: dynamically initialized during startup.
Logger global_logger;

// A safe entry point: it never touches global_logger before it is built.
void log_line(const std::string& message) {
    if (!logger_ready) {
        // Real code would call global_logger.log(...) on an object whose
        // constructor has not run yet: undefined behaviour.
        std::cout << "[logger] NOT CONSTRUCTED YET -> real code would be undefined behaviour\n";
        return;
    }
    global_logger.log(message);
}

}  // namespace tu_a

namespace tu_b {  // emulates config.cpp

struct Config {
    Config() {
        // Real code would call tu_a::global_logger.log(...) directly.
        tu_a::log_line("Initializing config");
    }

    std::string app_name = "Cppuccino";
};

// A namespace scope object: dynamically initialized during startup.
// Within this translation unit the definition order is guaranteed, so the
// logger above is built first and this initializer runs safely.
Config global_config;

}  // namespace tu_b

// --- 2. The fiasco, reported instead of executed ---

// A logger that has not been constructed yet is modelled with a flag, so the
// broken order can be shown without triggering undefined behaviour.
struct SimulatedLogger {
    bool ready = false;

    void log(const std::string& message) const {
        if (!ready) {
            std::cout << "  [fiasco] log(\"" << message
                      << "\") on an object whose constructor has not run: undefined behaviour\n";
            return;
        }
        std::cout << "  [fiasco] " << message << "\n";
    }
};

void simulateFiasco() {
    std::cout << "--- The fiasco, reported instead of executed ---\n";
    std::cout << "  linker order: config.cpp first, logger.cpp second\n";

    SimulatedLogger logger;             // the logger has not been constructed yet
    logger.log("Initializing config");  // this is what config.cpp's initializer does

    logger.ready = true;                // now logger.cpp's initializer runs
    logger.log("Initializing config");  // this one is fine
}

// --- 3. Fix: construct on first use (Meyers' singleton) ---

namespace safe {

struct Logger {
    Logger() {
        std::cout << "[init] safe::Logger constructed on first use\n";
    }

    ~Logger() {
        std::cout << "[shutdown] safe::Logger destroyed\n";
    }

    void log(const std::string& message) const {
        std::cout << "[logger] " << message << "\n";
    }
};

Logger& get_logger() {
    static Logger instance;  // function local static, initialized on first use
    return instance;
}

struct Config {
    Config() {
        get_logger().log("Initializing config");
    }

    ~Config() {
        // The logger was constructed first, so it is still alive here: the
        // function local statics are destroyed in reverse order of construction.
        get_logger().log("Config destroyed");
    }

    std::string app_name = "Cppuccino";
};

Config& get_config() {
    static Config instance;
    return instance;
}

}  // namespace safe

// --- 4. Fix: constant initialization ---

struct ConstConfig {
    const char* app_name;
};

// Constant initialized: this happens before any dynamic initialization, so it
// can never be affected by the initialization order of other statics.
constexpr ConstConfig constant_config{"Cppuccino"};

// C++20 offers constinit for a variable that must be constant initialized but
// still has to be modified at run time:
//
//     constinit int global_counter = 0;
//
// A dynamic initializer for a constinit variable is a compile error.

int main() {
    std::cout << "--- Startup order observed above ---\n";
    std::cout << "tu_b::global_config.app_name = " << tu_b::global_config.app_name << "\n";
    std::cout << "constant_config.app_name = " << constant_config.app_name << "\n";

    simulateFiasco();

    std::cout << "\n--- Fix: construct on first use ---\n";
    std::cout << "app_name = " << safe::get_config().app_name << "\n";

    std::cout << "\n=== All static initialization order scenarios completed ===\n";
    return 0;
}
