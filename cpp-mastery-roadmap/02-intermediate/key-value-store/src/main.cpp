#include "server/server.hpp"
#include "command/command_processor.hpp"
#include "store/store_manager.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <thread>

void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "Options:\n"
              << "  -h, --host <host>    Host to bind (default: 127.0.0.1)\n"
              << "  -p, --port <port>    Port to listen (default: 6379)\n"
              << "  -f, --file <path>    Persistence file (default: key_value_store.db)\n"
              << "  --cli                Run in CLI mode (no server)\n"
              << "  --help               Show this help\n";
}

void runCli(key_value_store::StoreManager& manager) {
    key_value_store::CommandProcessor processor(manager.store());

    std::cout << "key-value-store CLI mode\n";
    std::cout << "Commands: SET GET DELETE EXISTS COUNT CLEAR SAVE LOAD EXIT\n\n";

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        try {
            const auto result = processor.execute(line);

            if (result.action == key_value_store::CommandAction::load) {
                if (manager.load()) {
                    std::cout << "Loaded\n";
                } else {
                    std::cout << "Failed to load\n";
                }
            }

            if (result.persist_after_command && manager.autoPersist()) {
                manager.save();
            }

            std::cout << result.output << '\n';

            if (result.action == key_value_store::CommandAction::exit) {
                break;
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR " << e.what() << '\n';
        }
    }
}

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 6379;
    std::string persistencePath = "key_value_store.db";
    bool cliMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--host") {
            if (i + 1 < argc) host = argv[++i];
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) port = std::stoi(argv[++i]);
        } else if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc) persistencePath = argv[++i];
        } else if (arg == "--cli") {
            cliMode = true;
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }

    if (cliMode) {
        key_value_store::StoreManager manager(persistencePath);
        if (manager.load()) {
            std::cout << "Loaded data from " << persistencePath << "\n";
        }
        runCli(manager);
        return 0;
    }

    try {
        key_value_store::Server server(host, port, persistencePath);

        std::cout << "Press Ctrl+C to stop\n\n";

        std::thread serverThread([&server]() {
            server.start();
        });

        serverThread.join();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
