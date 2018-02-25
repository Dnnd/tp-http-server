
#include <string_view>
#include "defs.h"

std::string_view methodToString(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::HEAD:
            return "HEAD";
        default:
            return "";
    }
}

std::string_view codeName(HttpStatusCode code) {
    switch (code) {
        case OK:
            return "OK";
        case BadRequest:
            return "Bad Request";
        case Forbidden:
            return "Forbidden";
        case NotFound:
            return "Not Found";
        case NotImplemented:
            return "Not Implemented";
        case InternalError:
            return "Internal Server Error";
    }
}
