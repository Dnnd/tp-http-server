
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include "HttpParser.h"
#include <QDebug>

const std::unordered_map<HttpMethod, bool> HttpParser::implementationStatus = {
  {HttpMethod::GET,     true},
  {HttpMethod::HEAD,    true},

  {HttpMethod::POST,    false},
  {HttpMethod::PUT,     false},
  {HttpMethod::OPTIONS, false},
  {HttpMethod::DELETE,  false},
  {HttpMethod::PATCH,   false},
};

const std::unordered_map<std::string, HttpMethod> HttpParser::methodsCodes = {
  {"GET",     HttpMethod::GET},
  {"HEAD",    HttpMethod::HEAD},

  {"POST",    HttpMethod::POST},
  {"PUT",     HttpMethod::PUT},
  {"OPTIONS", HttpMethod::OPTIONS},
  {"DELETE",  HttpMethod::DELETE},
  {"PATCH",   HttpMethod::PATCH},
};

const QMimeDatabase HttpParser::mimeDb{};

HttpParser::HttpParser(QString documentRoot, QByteArray *buffer)
  : buffer_{buffer},
    pos{0},
    uriBegin_{0},
    document_{documentRoot},
    externalState_{ExternalState::Processing},
    internalState_{InternalState::None},
    versionRe_{R"(^HTTP/1\.[01]{1}$)"} {
}

HttpParser::ExternalState HttpParser::parse() {
    switch (internalState_) {
        case InternalState::None:
            return startProcessing();
        case InternalState::Method:
            return parseMethod();
        case InternalState::Uri:
            return parseUri();
        case InternalState::Version:
            return parseVersion();
    }
}

HttpParser::ExternalState HttpParser::startProcessing() {
    if (pos != 0 || buffer_ == nullptr) {
        return setErrorCode(InternalError);
    }
    internalState_ = InternalState::Method;
    return parseMethod();
}

HttpParser::ExternalState HttpParser::parseMethod() {
    if (buffer_ == nullptr) {
        return setErrorCode(InternalError);
    }

    if (buffer_->length() == pos) {
        return ExternalState::Processing;
    }

    for (; pos < buffer_->length(); ++pos) {
        if (buffer_->at(pos) == ' ') {
            return switchToUriParsing();
        }
    }

    return ExternalState::Processing;
}

HttpParser::ExternalState HttpParser::switchToUriParsing() {
    if (buffer_->length() <= pos) {
        return ExternalState::Processing;
    }
    auto methodStatusIter = methodsCodes.find(buffer_->mid(0, pos).toStdString());

    if (methodStatusIter == methodsCodes.end()) {
        return setErrorCode(HttpStatusCode::BadRequest);
    }


    auto[methodName, methodCode] = *methodStatusIter;

    auto[code, isImplemented] = *implementationStatus.find(methodCode);

    if (!isImplemented) {
        setErrorCode(HttpStatusCode::NotImplemented);
    }

    if (buffer_->at(static_cast<int>(pos)) == ' ') {
        uriBegin_ = ++pos;
        internalState_ = InternalState::Uri;
        requestInfo_.method = methodCode;
        return parseUri();
    }

    return ExternalState::Error;
}

HttpParser::ExternalState HttpParser::parseUri() {
    if (buffer_ == nullptr) {
        return setErrorCode(InternalError);
    }

    if (buffer_->length() == pos) {
        return ExternalState::Processing;
    }

    for (; pos < buffer_->length(); ++pos) {
        if (buffer_->at(pos) == ' ') {
            versionBegin_ = ++pos;
            return switchToVersionParsing();
        }
    }

    return ExternalState::Processing;

}

HttpParser::ExternalState HttpParser::switchToVersionParsing() {

    QUrl url(buffer_->mid(uriBegin_, (pos - 1) - uriBegin_).prepend("./"));

    if (!url.isValid()) {
        return setErrorCode(HttpStatusCode::BadRequest);
    }

    auto path = url.path();

    if (document_.relativeFilePath(path).startsWith("..")) {
        qDebug() << document_.relativeFilePath(path);
        setErrorCode(HttpStatusCode::Forbidden);
        return parseVersion();
    }

    if (!document_.exists(path)) {
        setErrorCode(HttpStatusCode::NotFound);
        return parseVersion();
    }
    requestInfo_.file = document_.absoluteFilePath(path);

    QFileInfo fileInfo{requestInfo_.file};

    if (fileInfo.isDir()) {
        requestInfo_.file.append("index.html");
    }
    fileInfo.setFile(requestInfo_.file);


    QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo.fileName());

    if (mimeType.inherits("application/x-extension-html")) {
        requestInfo_.mimeType = "text/html";
    } else if (mimeType.inherits("application/vnd.adobe.flash.movie")) {
        requestInfo_.mimeType = "application/x-shockwave-flash";
    } else {
        requestInfo_.mimeType = mimeType.name();
    }

    requestInfo_.size = static_cast<size_t>(fileInfo.size());
    internalState_ = InternalState::Version;
    return parseVersion();

}

HttpParser::ExternalState HttpParser::parseVersion() {
    if (buffer_ == nullptr) {

        return setErrorCode(InternalError);
    }

    if (buffer_->length() == pos) {
        return ExternalState::Processing;
    }

    for (; pos < buffer_->length(); ++pos) {
        if (buffer_->at(pos) == '\n') {
            requestInfo_.version = buffer_->mid(versionBegin_, (pos - 1) - versionBegin_);
            if (!versionRe_.exactMatch(requestInfo_.version)) {
                setErrorCode(HttpStatusCode::BadRequest);
            }
            return parseRequestEnd();
        }
    }

    return ExternalState::Processing;
}

HttpParser::ExternalState HttpParser::setErrorCode(HttpStatusCode code) {
    errorCode_ = code;
    return ExternalState::Error;
}

HttpStatusCode HttpParser::getErrorCode() const {
    return errorCode_;
}

HttpRequestInfo HttpParser::takeRequestInfo() {
    return std::move(requestInfo_);
}

HttpParser::ExternalState HttpParser::parseRequestEnd() {

    for (; pos < buffer_->length() - 1; ++pos) {
        if (buffer_->at(pos) == '\n'
            && buffer_->at(pos + 1) == '\r') {
            if (errorCode_ == 200) {
                return ExternalState::Finished;
            } else {
                return ExternalState::Error;
            }
        }
    }

    return ExternalState::Processing;
}


