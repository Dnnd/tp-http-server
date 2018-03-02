
#include <QtCore/QDateTime>
#include <QtCore/QBuffer>
#include "HttpHandler.h"
#include <QDebug>

namespace {
  const char *CRLF = "\r\n";
}

const QString HttpHandler::dateformat{"ddd',' d MMM yyyy hh:mm:ss 'GWT'"};

HttpHandler::HttpHandler(const QString &documentRoot, QTcpSocket *socket, QObject *parent)
  : Handler{socket, parent},
    buffer_{},
    parser_{documentRoot, &buffer_},
    source{nullptr},
    outputBuffer_{CHUNK_SIZE * 2} {
}

void HttpHandler::handleNewData() {
    buffer_.append(socket_->readAll());
    switch (parser_.parse()) {
        case HttpParser::ExternalState::Processing:
            break;

        case HttpParser::ExternalState::Error:
            requestInfo_ = parser_.takeRequestInfo();
            writeResponseString(parser_.getErrorCode());
            qInfo() << parser_.getErrorCode();
            writeDefaultHeaders();
            sendDefaultResponse();
            break;

        case HttpParser::ExternalState::Finished:
            qInfo() << "Success: " << buffer_;
            requestInfo_ = parser_.takeRequestInfo();
            writeResponseString(parser_.getErrorCode());
            writeDefaultHeaders();
            if (requestInfo_.method == HttpMethod::GET) {
                processGetRequest();
            } else if (requestInfo_.method == HttpMethod::HEAD) {
                processHeadRequest();
            } else {
                sendDefaultResponse();
            }
            break;
    }
}

void HttpHandler::handleDisconnect() {
    socket_->deleteLater();
    emit finish(this);
}

void HttpHandler::handleError(QTcpSocket::SocketError error) {
    qInfo() << socket_->errorString();
    socket_->deleteLater();
}

void HttpHandler::closeConnection() {
    socket_->close();
}

void HttpHandler::processGetRequest() {
    source = new QFile{requestInfo_.file};
    source->setParent(this);

    if (!source->open(QFile::ReadOnly)) {
        flushResponseMetaInfo();
        asyncFlushBuffer();
        return;
    }

    writeHeader("Content-Length", std::to_string(source->size()));
    writeHeader("Content-Type", requestInfo_.mimeType);
    flushResponseMetaInfo();
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncReadFile);
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncWriteToSocket);
    QObject::connect(socket_, &QTcpSocket::bytesWritten, this, &HttpHandler::countSentBytes);
    asyncExecutor_.start();
}

void HttpHandler::asyncWriteToSocket() {


    auto[bytesToEnd, bytesFromBegin] = outputBuffer_.totalBytesAvailableToRead();

    if (bytesToEnd + bytesFromBegin == 0) {
        return;
    }

    outputBuffer_.readFromBuffer(socket_, bytesToEnd + bytesFromBegin);

    if (bytesSent_ == -1) {
        qInfo() << socket_->errorString();
        closeConnection();
    }

}

void HttpHandler::asyncReadFile() {
    if (source->atEnd()) {
        return;
    }

    bytesWritten_ += outputBuffer_.writeToBuffer(source, CHUNK_SIZE);

    qInfo() << "written" << bytesWritten_;

    if (source->atEnd()) {
        qInfo() << "all bytes in buffer";
        QObject::disconnect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncReadFile);
        allBytesInBuffer = true;
    }

}

void HttpHandler::processHeadRequest() {
    QFileInfo fileInfo{requestInfo_.file};
    writeHeader("Content-Length", std::to_string(fileInfo.size()));
    writeHeader("Content-Type", requestInfo_.mimeType);
    flushResponseMetaInfo();
    asyncFlushBuffer();

    asyncExecutor_.start();
}

void HttpHandler::writeHeader(std::string headerName, std::string headerValue) {
    responseMetaInfoBuffer_.append(headerName.data(), headerName.size());
    responseMetaInfoBuffer_.append(": ");
    responseMetaInfoBuffer_.append(headerValue.data(), headerValue.size());
    responseMetaInfoBuffer_.append(CRLF);
}

void HttpHandler::writeHeader(std::string headerName, const QString &headerValue) {
    responseMetaInfoBuffer_.append(headerName.data(), headerName.size());
    responseMetaInfoBuffer_.append(": ");
    responseMetaInfoBuffer_.append(headerValue);
    responseMetaInfoBuffer_.append(CRLF);
}

void HttpHandler::writeDefaultHeaders() {
    writeHeader("Server", std::string("tp-http-server"));
    writeHeader("Date", QLocale().toString(QDateTime::currentDateTimeUtc(), dateformat));
    writeHeader("Connection", std::string("close"));
}

void HttpHandler::writeResponseString(HttpStatusCode code) {
    responseMetaInfoBuffer_.append(requestInfo_.version);
    responseMetaInfoBuffer_.append(' ');
    responseMetaInfoBuffer_.append(std::to_string(code).data());
    responseMetaInfoBuffer_.append(' ');
    responseMetaInfoBuffer_.append(codeName(code).data());
    responseMetaInfoBuffer_.append(CRLF);

}

void HttpHandler::countSentBytes(qint64 sentChunk) {
    bytesSent_ += sentChunk;
    qInfo() << "sent " << sentChunk << "total" << bytesSent_ << "written" << bytesWritten_;
    if (allBytesInBuffer && bytesSent_ == bytesWritten_) {
        qInfo() << "done: bytes written " << bytesWritten_;
        qInfo() << "done: bytes sent " << bytesSent_;
        asyncExecutor_.stop();
        closeConnection();
        emit finish(this);
    }
}

void HttpHandler::flushResponseMetaInfo() {
    responseMetaInfoBuffer_.append(CRLF);
    QBuffer buffWrapper{&responseMetaInfoBuffer_};
    buffWrapper.open(QBuffer::OpenModeFlag::ReadWrite);
    bytesWritten_ += outputBuffer_.writeToBuffer(&buffWrapper, responseMetaInfoBuffer_.size());
}

void HttpHandler::asyncFlushBuffer() {
    allBytesInBuffer = true;
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncWriteToSocket);
    QObject::connect(socket_, &QTcpSocket::bytesWritten, this, &HttpHandler::countSentBytes);
    asyncExecutor_.start();
}

void HttpHandler::sendDefaultResponse() {
    flushResponseMetaInfo();
    asyncFlushBuffer();
}





