#include "http.hpp"

#include <algorithm>
#include <cctype>

namespace http {

namespace {

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace

// --- Request ---

bool Request::parse(const std::string& raw) {
    std::istringstream stream(raw);
    std::string line;

    // Parse request line
    if (!std::getline(stream, line)) return false;
    line = trim(line);

    std::istringstream requestLine(line);
    std::string methodStr, path, version;
    if (!(requestLine >> methodStr >> path >> version)) return false;

    method = parseMethod(methodStr);
    this->path = path;
    this->version = version;

    // Parse headers
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty()) break;

        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string name = trim(line.substr(0, colon));
        std::string value = trim(line.substr(colon + 1));
        headers[toLower(name)] = value;
    }

    // Parse body
    std::string bodyLine;
    while (std::getline(stream, bodyLine)) {
        if (!body.empty()) body += "\n";
        body += bodyLine;
    }

    return true;
}

std::string Request::getHeader(const std::string& name) const {
    auto it = headers.find(toLower(name));
    return (it != headers.end()) ? it->second : "";
}

std::string Request::methodName() const {
    switch (method) {
        case Method::GET: return "GET";
        case Method::POST: return "POST";
        case Method::PUT: return "PUT";
        case Method::DELETE: return "DELETE";
        case Method::HEAD: return "HEAD";
        case Method::OPTIONS: return "OPTIONS";
        case Method::PATCH: return "PATCH";
        default: return "UNKNOWN";
    }
}

// --- Response ---

void Response::setStatus(int code, const std::string& message) {
    statusCode = code;
    statusMessage = message;
}

void Response::setHeader(const std::string& name, const std::string& value) {
    headers[toLower(name)] = value;
}

void Response::setBody(const std::string& content) {
    body = content;
    headers["content-length"] = std::to_string(body.size());
}

std::string Response::serialize() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    for (const auto& [name, value] : headers) {
        oss << name << ": " << value << "\r\n";
    }

    oss << "\r\n";
    oss << body;

    return oss.str();
}

Response Response::ok(const std::string& body) {
    Response resp;
    resp.setStatus(200, "OK");
    resp.setBody(body);
    return resp;
}

Response Response::notFound() {
    Response resp;
    resp.setStatus(404, "Not Found");
    resp.setBody("Not Found");
    return resp;
}

Response Response::badRequest(const std::string& error) {
    Response resp;
    resp.setStatus(400, "Bad Request");
    resp.setBody(error.empty() ? "Bad Request" : error);
    return resp;
}

Response Response::internalError(const std::string& error) {
    Response resp;
    resp.setStatus(500, "Internal Server Error");
    resp.setBody(error.empty() ? "Internal Server Error" : error);
    return resp;
}

// --- Utilities ---

Method parseMethod(const std::string& str) {
    if (str == "GET") return Method::GET;
    if (str == "POST") return Method::POST;
    if (str == "PUT") return Method::PUT;
    if (str == "DELETE") return Method::DELETE;
    if (str == "HEAD") return Method::HEAD;
    if (str == "OPTIONS") return Method::OPTIONS;
    if (str == "PATCH") return Method::PATCH;
    return Method::Unknown;
}

std::string statusMessage(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}

} // namespace http
