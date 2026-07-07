#pragma once

#include <string>
#include <unordered_map>
#include <sstream>

namespace http {

enum class Method {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    PATCH,
    Unknown
};

enum class StatusCode {
    OK = 200,
    Created = 201,
    BadRequest = 400,
    NotFound = 404,
    MethodNotAllowed = 405,
    InternalServerError = 500
};

struct Request {
    Method method = Method::Unknown;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    bool parse(const std::string& raw);
    std::string getHeader(const std::string& name) const;
    std::string methodName() const;
};

struct Response {
    int statusCode = 200;
    std::string statusMessage = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    void setStatus(int code, const std::string& message);
    void setHeader(const std::string& name, const std::string& value);
    void setBody(const std::string& content);
    std::string serialize() const;

    static Response ok(const std::string& body = "");
    static Response notFound();
    static Response badRequest(const std::string& error = "");
    static Response internalError(const std::string& error = "");
};

Method parseMethod(const std::string& str);
std::string statusMessage(int code);

} // namespace http
