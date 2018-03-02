#ifndef TP_HTTP_SERVER_DEFS_H
#define TP_HTTP_SERVER_DEFS_H


#include <string_view>

enum HttpStatusCode {
    OK = 200,
    BadRequest = 400,
    Forbidden = 403,
    NotFound = 404,
    NotImplemented = 405,
    InternalError = 500,
};

enum class HttpMethod {
    GET,
    HEAD,
    POST,
    PUT,
    PATCH,
    DELETE,
    OPTIONS
};

std::string_view methodToString(HttpMethod method);
std::string_view codeName(HttpStatusCode code);
#endif //TP_HTTP_SERVER_DEFS_H
