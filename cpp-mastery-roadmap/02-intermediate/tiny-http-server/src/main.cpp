#include "server/server.hpp"

int main() {

    tiny_http_server::Server server("0.0.0.0", 8082);

    server.start();

    return 0;
}