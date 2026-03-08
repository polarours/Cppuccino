#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace tiny_http_server {

class Server {
    public:
        Server(const std::string& addr, int port);
        ~Server();
        void start();

    private:
        int port_;
        std::string addr_;
        int server_fd_;

        void handle_client(int client_fd);
        void setup_socket();
        void cleanup();
};

} // namespace tiny_http_server