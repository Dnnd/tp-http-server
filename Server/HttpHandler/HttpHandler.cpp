
#include <QtCore/QDateTime>
#include "HttpHandler.h"
namespace {
    const char * CRLF = "\r\n";
}
const QString HttpHandler::dateformat{"ddd',' d MMM yyyy hh:mm:ss 'GWT'"};

HttpHandler::HttpHandler(const QString &documentRoot, QTcpSocket *socket, QObject *parent)
        : Handler{socket, parent},
          buffer_{},
          parser_{documentRoot, &buffer_},
          source{nullptr} {

}

void HttpHandler::handleNewData() {
    buffer_.append(socket_->readAll());
    switch (parser_.parse()) {
        case HttpParser::ExternalState::Processing:
            break;
        case HttpParser::ExternalState::Error:
            qDebug() << "Error: invalid http header " << buffer_;
            writeResponseString(parser_.getErrorCode());
            writeDefaultHeaders();
            closeConnection();
            emit finish(this);
            break;
        case HttpParser::ExternalState::Finished:
            qDebug() << "Success: " << buffer_;
            requestInfo_ = parser_.takeRequestInfo();
            writeResponseString(parser_.getErrorCode());
            writeDefaultHeaders();
            if (requestInfo_.method == HttpMethod::GET) {
                processGetRequest();
            } else if (requestInfo_.method == HttpMethod::HEAD) {
                processHeadRequest();
            }
            break;
    }
}

void HttpHandler::handleDisconnect() {
    closeConnection();
}

void HttpHandler::handleError(QTcpSocket::SocketError error) {
    qDebug() << socket_->errorString();
    socket_->deleteLater();
}

void HttpHandler::closeConnection() {
    socket_->deleteLater();
    emit finish(this);
}

void HttpHandler::processGetRequest() {
    source = new QFile{requestInfo_.file};
    source->setParent(this);

    if (!source->open(QFile::ReadOnly)) {
        closeConnection();
    }

    writeHeader("Content-Length", std::to_string(source->size()));
    writeHeader("Content-Type", requestInfo_.mimeType);

    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncReadFile);
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncWriteToSocket);
    QObject::connect(socket_, &QTcpSocket::bytesWritten, this, &HttpHandler::countSentBytes);
    asyncExecutor_.start();
}

void HttpHandler::asyncWriteToSocket() {
    if (bytesWritten_ >= outputBuffer_.length()) {
        return;
    }
    char *from = outputBuffer_.data() + bytesWritten_;
    auto chunkWritten = socket_->write(from);
    if (chunkWritten == -1) {
        qDebug() << socket_->errorString();
        closeConnection();
    }
    bytesWritten_ += chunkWritten;
}

void HttpHandler::asyncReadFile() {
    if (source->atEnd()) {
        return;
    }
    outputBuffer_.append(source->read(CHUNK_SIZE));
    if (source->atEnd()) {
        QObject::disconnect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncReadFile);
        allBytesInBuffer = true;
    }
}

void HttpHandler::processHeadRequest() {

}

void HttpHandler::writeHeader(std::string headerName, std::string headerValue) {
    outputBuffer_.append(headerName.data(), headerName.size());
    outputBuffer_.append(": ");
    outputBuffer_.append(headerValue.data(), headerValue.size());
    outputBuffer_.append(CRLF);
}

void HttpHandler::writeHeader(std::string headerName, const QString &headerValue) {
    outputBuffer_.append(headerName.data(), headerName.size());
    outputBuffer_.append(": ");
    outputBuffer_.append(headerValue);
    outputBuffer_.append(CRLF);
}

void HttpHandler::writeDefaultHeaders() {
    writeHeader("Server", std::string("tp-http-server"));
    writeHeader("Date", QLocale().toString(QDateTime::currentDateTimeUtc(), dateformat));
    writeHeader("Connection", std::string("close"));
}

void HttpHandler::writeResponseString(HttpStatusCode code) {
    outputBuffer_.append(requestInfo_.version);
    outputBuffer_.append(' ');
    outputBuffer_.append(std::to_string(code).data());
    outputBuffer_.append(' ');
    outputBuffer_.append(codeName(code).data());
    outputBuffer_.append(CRLF);

}

void HttpHandler::countSentBytes(qint64 sentChunk) {
    bytesSent_ += sentChunk;

    if (allBytesInBuffer && bytesSent_ >= outputBuffer_.size()) {
        qDebug() << "done: bytes written " << bytesWritten_;
        qDebug() << "done: bytes sent " << bytesSent_;
        qDebug() << "Output Buffer: " << outputBuffer_;
        asyncExecutor_.stop();
        closeConnection();
        emit finish(this);
    }
}




