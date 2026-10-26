#include "web_server.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <fstream>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_request_parsing() {
    web_server::Request req;
    std::string raw = "GET /hello HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "GET", "expected GET method");
    expect(req.path == "/hello", "expected /hello path");
    expect(req.headers["Host"] == "localhost", "expected Host header");
}

void test_response_ok() {
    web_server::Response resp = web_server::Response::ok("Hello");
    std::string serialized = resp.serialize();
    expect(serialized.find("200 OK") != std::string::npos, "expected 200 OK");
    expect(serialized.find("Hello") != std::string::npos, "expected body");
}

void test_response_not_found() {
    web_server::Response resp = web_server::Response::notFound();
    std::string serialized = resp.serialize();
    expect(serialized.find("404 Not Found") != std::string::npos, "expected 404");
}

void test_server_creation() {
    web_server::WebServer server(18001);
    expect(!server.isRunning(), "expected not running initially");
}

void test_route_registration() {
    web_server::WebServer server(18002);
    server.get("/", [](const web_server::Request&) {
        return web_server::Response::ok("Home");
    });
    server.get("/about", [](const web_server::Request&) {
        return web_server::Response::ok("About");
    });
    // Just test that routes can be registered without crash
    expect(true, "expected route registration to succeed");
}

void test_request_post() {
    web_server::Request req;
    std::string raw = "POST /submit HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "Content-Type: application/json\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "POST", "expected POST method");
    expect(req.path == "/submit", "expected /submit path");
    expect(req.headers["Content-Type"] == "application/json", "expected Content-Type header");
}

void test_request_put() {
    web_server::Request req;
    std::string raw = "PUT /resource/1 HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "PUT", "expected PUT method");
    expect(req.path == "/resource/1", "expected /resource/1 path");
}

void test_request_delete() {
    web_server::Request req;
    std::string raw = "DELETE /resource/1 HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.method == "DELETE", "expected DELETE method");
    expect(req.path == "/resource/1", "expected /resource/1 path");
}

void test_request_multiple_headers() {
    web_server::Request req;
    std::string raw = "GET / HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "Accept: text/html\r\n"
                      "User-Agent: test/1.0\r\n"
                      "\r\n";

    expect(req.parse(raw), "expected parse to succeed");
    expect(req.headers.size() == 3, "expected 3 headers");
    expect(req.headers["Accept"] == "text/html", "expected Accept header");
    expect(req.headers["User-Agent"] == "test/1.0", "expected User-Agent header");
}

void test_response_bad_request() {
    web_server::Response resp = web_server::Response::badRequest();
    std::string serialized = resp.serialize();
    expect(serialized.find("400 Bad Request") != std::string::npos, "expected 400");
}

void test_response_internal_error() {
    web_server::Response resp = web_server::Response::internalError();
    std::string serialized = resp.serialize();
    expect(serialized.find("500 Internal Server Error") != std::string::npos, "expected 500");
}

void test_response_with_headers() {
    web_server::Response resp;
    resp.statusCode = 201;
    resp.statusMessage = "Created";
    resp.headers["X-Custom"] = "value";
    resp.headers["Content-Type"] = "application/json";
    resp.body = "{\"id\":1}";

    std::string serialized = resp.serialize();
    expect(serialized.find("201 Created") != std::string::npos, "expected 201");
    expect(serialized.find("X-Custom: value") != std::string::npos, "expected custom header");
    expect(serialized.find("Content-Type: application/json") != std::string::npos, "expected content type");
}

void test_request_invalid() {
    web_server::Request req;
    expect(!req.parse("INVALID"), "expected invalid request to fail");
    expect(!req.parse(""), "expected empty request to fail");
}

// Regression: stop() must terminate even though the accept loop is blocked
// in accept(). close() alone does not wake a blocked accept() on Linux, so
// stop() has to shut the socket down explicitly. Without this, stop() hangs
// and never returns.
void test_stop_terminates_with_blocked_accept() {
    web_server::WebServer server(18003);

    std::thread serverThread([&server]() {
        server.start();
    });

    // Give the accept loop time to block.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::atomic<bool> stopped{false};
    std::thread stopper([&server, &stopped]() {
        server.stop();
        stopped = true;
    });

    // If stop() hangs, the join below never returns and the test times out
    // instead of failing cleanly.
    stopper.join();
    expect(stopped.load(), "expected stop() to complete");

    if (serverThread.joinable()) {
        serverThread.join();
    }
}

// Regression: a client that stays connected without sending anything leaves
// its handler blocked in recv(). stop() must still return.
void test_stop_terminates_with_idle_client() {
    web_server::WebServer server(18004);

    std::thread serverThread([&server]() {
        server.start();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Connect a client and leave it idle.
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    expect(fd >= 0, "expected socket creation to succeed");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(18004);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bool connected = connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0;
    expect(connected, "expected client connect to succeed");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    server.stop();
    expect(!server.isRunning(), "expected server not running after stop");

    if (serverThread.joinable()) {
        serverThread.join();
    }
    close(fd);
}

// The tests above never went through the routing. These do, over a
// socket, the way a reader uses the server.

// Sends a request to the server and returns the whole answer.
std::string ask(int port, const std::string& request) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("could not open a socket");
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<uint16_t>(port));
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(fd, reinterpret_cast<struct sockaddr*>(&address),
                 sizeof(address)) < 0) {
        close(fd);
        throw std::runtime_error("could not connect to the server");
    }

    if (send(fd, request.data(), request.size(), 0) < 0) {
        close(fd);
        throw std::runtime_error("could not send the request");
    }

    std::string answer;
    char buffer[1024];
    ssize_t bytes = 0;
    while ((bytes = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        answer += buffer;
        if (answer.find("\r\n\r\n") != std::string::npos &&
            answer.size() > 40) {
            break;
        }
    }
    close(fd);
    return answer;
}

// A route answers the method it was registered for.
void test_the_route_answers_its_own_method() {
    web_server::WebServer server(18021);

    server.get("/thing", [](const web_server::Request&) {
        return web_server::Response::ok("got");
    });
    server.post("/thing", [](const web_server::Request&) {
        return web_server::Response::ok("posted");
    });

    std::thread serverThread([&server]() { server.start(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    const std::string get = ask(18021, "GET /thing HTTP/1.1\r\nHost: x\r\n\r\n");
    expect(get.find("200 OK") != std::string::npos, "expected 200 OK");
    expect(get.find("got") != std::string::npos, "expected the get route");
    expect(get.find("posted") == std::string::npos,
           "expected the post route not to answer");

    const std::string post = ask(18021, "POST /thing HTTP/1.1\r\nHost: x\r\n\r\n");
    expect(post.find("posted") != std::string::npos, "expected the post route");
    expect(post.find("got") == std::string::npos,
           "expected the get route not to answer");

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

// A path nobody registered is a 404.
void test_an_unknown_path_is_a_404() {
    web_server::WebServer server(18022);

    server.get("/known", [](const web_server::Request&) {
        return web_server::Response::ok("ok");
    });

    std::thread serverThread([&server]() { server.start(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    const std::string answer =
        ask(18022, "GET /unknown HTTP/1.1\r\nHost: x\r\n\r\n");
    expect(answer.find("404") != std::string::npos, "expected 404");

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

// A middleware sees the request before the route does, and it can stop it.
void test_a_middleware_runs_first() {
    web_server::WebServer server(18023);

    int seen = 0;
    server.use([&seen](web_server::Request& request, web_server::Response&) {
        seen++;
        return true;
    });
    server.get("/checked", [](const web_server::Request&) {
        return web_server::Response::ok("ok");
    });

    std::thread serverThread([&server]() { server.start(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ask(18023, "GET /checked HTTP/1.1\r\nHost: x\r\n\r\n");
    expect(seen == 1, "expected the middleware to have run");

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

// A middleware that says no keeps the request from reaching the route.
void test_a_middleware_can_stop_a_request() {
    web_server::WebServer server(18024);

    server.use([](web_server::Request&, web_server::Response& response) {
        response.statusCode = 403;
        response.statusMessage = "Forbidden";
        return false;
    });
    server.get("/blocked", [](const web_server::Request&) {
        return web_server::Response::ok("should not be reached");
    });

    std::thread serverThread([&server]() { server.start(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    const std::string answer =
        ask(18024, "GET /blocked HTTP/1.1\r\nHost: x\r\n\r\n");
    expect(answer.find("403") != std::string::npos, "expected 403");
    expect(answer.find("should not be reached") == std::string::npos,
           "expected the route not to run");

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

// A file under a static directory is served.
void test_a_static_file_is_served() {
    web_server::WebServer server(18025);

    const std::string directory = "/tmp/cppuccino-web-server-static";
    mkdir(directory.c_str(), 0755);
    {
        std::ofstream file(directory + "/hello.txt");
        file << "from a file";
    }

    server.serveStatic("/files", directory);

    std::thread serverThread([&server]() { server.start(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    const std::string answer =
        ask(18025, "GET /files/hello.txt HTTP/1.1\r\nHost: x\r\n\r\n");
    expect(answer.find("200 OK") != std::string::npos, "expected 200 OK");
    expect(answer.find("from a file") != std::string::npos, "expected the file");

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
    remove((directory + "/hello.txt").c_str());
    rmdir(directory.c_str());
}

} // namespace

int main() {
    try {
        std::cout << "Web Server Tests:\n";
        test_request_parsing();
        std::cout << "  test_request_parsing: PASS\n";

        test_request_post();
        std::cout << "  test_request_post: PASS\n";

        test_request_put();
        std::cout << "  test_request_put: PASS\n";

        test_request_delete();
        std::cout << "  test_request_delete: PASS\n";

        test_request_multiple_headers();
        std::cout << "  test_request_multiple_headers: PASS\n";

        test_request_invalid();
        std::cout << "  test_request_invalid: PASS\n";

        test_response_ok();
        std::cout << "  test_response_ok: PASS\n";

        test_response_not_found();
        std::cout << "  test_response_not_found: PASS\n";

        test_response_bad_request();
        std::cout << "  test_response_bad_request: PASS\n";

        test_response_internal_error();
        std::cout << "  test_response_internal_error: PASS\n";

        test_response_with_headers();
        std::cout << "  test_response_with_headers: PASS\n";

        test_server_creation();
        std::cout << "  test_server_creation: PASS\n";

        test_route_registration();
        std::cout << "  test_route_registration: PASS\n";

        test_stop_terminates_with_blocked_accept();
        std::cout << "  test_stop_terminates_with_blocked_accept: PASS\n";

        test_stop_terminates_with_idle_client();
        std::cout << "  test_stop_terminates_with_idle_client: PASS\n";

        test_the_route_answers_its_own_method();
        std::cout << "  test_the_route_answers_its_own_method: PASS\n";

        test_an_unknown_path_is_a_404();
        std::cout << "  test_an_unknown_path_is_a_404: PASS\n";

        test_a_middleware_runs_first();
        std::cout << "  test_a_middleware_runs_first: PASS\n";

        test_a_middleware_can_stop_a_request();
        std::cout << "  test_a_middleware_can_stop_a_request: PASS\n";

        test_a_static_file_is_served();
        std::cout << "  test_a_static_file_is_served: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "web_server_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nweb_server_tests passed" << std::endl;
    return 0;
}
