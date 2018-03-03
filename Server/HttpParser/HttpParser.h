#ifndef TP_HTTP_SERVER_HTTPPARSER_H
#define TP_HTTP_SERVER_HTTPPARSER_H


#include <QtCore/QArgument>
#include <array>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <unordered_map>
#include <QtCore/QMimeDatabase>
#include <unordered_set>
#include "defs.h"
#include "HttpRequestInfo.h"

class HttpParser {

public:


    enum class ExternalState {
        Processing,
        Error,
        Finished,
    };

    enum class InternalState {
        None,
        Method,
        Uri,
        Version
    };

    explicit HttpParser(QString documentRoot, QByteArray *buffer);


    ExternalState parse();

    HttpStatusCode getErrorCode() const;

    HttpRequestInfo takeRequestInfo();

private:
    ExternalState startProcessing();

    ExternalState parseMethod();

    ExternalState switchToUriParsing();

    ExternalState parseUri();

    ExternalState switchToVersionParsing();

    ExternalState parseVersion();

private:
    QByteArray *buffer_;

    std::size_t pos;

    std::size_t uriBegin_;
    std::size_t versionBegin_;

    QDir document_;

    HttpRequestInfo requestInfo_;

    ExternalState externalState_;
    InternalState internalState_;

    HttpStatusCode errorCode_{HttpStatusCode::OK};

    static const std::unordered_map<std::string, HttpMethod > methodsCodes;

    static const std::unordered_map<HttpMethod, bool > implementationStatus;

    const QRegExp versionRe_;

    static const QMimeDatabase mimeDb;

    ExternalState setErrorCode(HttpStatusCode code);

    ExternalState parseRequestEnd();
};


#endif //TP_HTTP_SERVER_HTTPPARSER_H
