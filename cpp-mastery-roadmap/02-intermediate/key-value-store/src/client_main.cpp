#include "client/client.hpp"
#include "protocol/protocol.hpp"

#include <iostream>
#include <string>

void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " [options] [command]\n"
              << "Options:\n"
              << "  -h, --host <host>    Server host (default: 127.0.0.1)\n"
              << "  -p, --port <port>    Server port (default: 6379)\n"
              << "  -n, --no-echo        Disable response formatting\n"
              << "  --help               Show this help\n"
              << "\nExamples:\n"
              << "  " << prog << "                          # Interactive mode\n"
              << "  " << prog << " SET key value            # Single command\n"
              << "  " << prog << " GET key                  # Get value\n"
              << "  " << prog << " PING                     # Ping server\n";
}

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 6379;
    bool echo = true;
    std::string singleCommand;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--host") {
            if (i + 1 < argc) host = argv[++i];
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) port = std::stoi(argv[++i]);
        } else if (arg == "-n" || arg == "--no-echo") {
            echo = false;
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg[0] != '-') {
            singleCommand = arg;
            for (int j = i + 1; j < argc; ++j) {
                singleCommand += " ";
                singleCommand += argv[j];
            }
            break;
        }
    }

    key_value_store::Client client(host, port);
    client.setEcho(echo);

    if (!client.connect()) {
        return 1;
    }

    if (!singleCommand.empty()) {
        key_value_store::ProtocolResponse response = client.execute(singleCommand);

        if (echo) {
            switch (response.status) {
                case key_value_store::ProtocolStatus::ok:
                    if (!response.data.empty()) {
                        std::cout << response.data << "\n";
                    } else {
                        std::cout << "OK\n";
                    }
                    break;
                case key_value_store::ProtocolStatus::value:
                    std::cout << response.data << "\n";
                    break;
                case key_value_store::ProtocolStatus::count:
                    std::cout << "(integer) " << response.data << "\n";
                    break;
                case key_value_store::ProtocolStatus::not_found:
                    std::cout << "(nil)\n";
                    break;
                case key_value_store::ProtocolStatus::error:
                    std::cout << "(error) " << response.data << "\n";
                    break;
                case key_value_store::ProtocolStatus::bye:
                    break;
            }
        }

        client.execute("QUIT");
        return 0;
    }

    client.interactive();
    return 0;
}
