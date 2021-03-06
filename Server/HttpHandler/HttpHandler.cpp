
#include <QtCore/QDateTime>
#include <QtCore/QBuffer>
#include "HttpHandler.h"

namespace {
  const char *CRLF = "\r\n";
  const int MAX_RECIEVE_BUFFER_SIZE = 4 * 1024;
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
    if (buffer_.size() >= MAX_RECIEVE_BUFFER_SIZE) {
        closeConnection();
        return;
    }
    switch (parser_.parse()) {
        case HttpParser::ExternalState::Processing:
            break;

        case HttpParser::ExternalState::Error:
            QObject::disconnect(socket_, &QTcpSocket::readyRead, this, &HttpHandler::handleNewDataSlot);
            requestInfo_ = parser_.takeRequestInfo();
            writeResponseString(parser_.getErrorCode());
            qDebug() << parser_.getErrorCode();
            writeDefaultHeaders();
            sendDefaultResponse();
            break;

        case HttpParser::ExternalState::Finished:
            qDebug() << "Success: " << buffer_;

            QObject::disconnect(socket_, &QTcpSocket::readyRead, this, &HttpHandler::handleNewDataSlot);
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
    qDebug() << socket_->errorString();
    socket_->deleteLater();
    socket_->close();
}

void HttpHandler::closeConnection() {
    socket_->close();
}

void HttpHandler::processGetRequest() {
    if (source == nullptr) {
        source = new QFile{requestInfo_.file};
        source->setParent(this);
    }

    if (!source->open(QFile::ReadOnly | QFile::Unbuffered)) {
        QTimer::singleShot(0, this, [this] {
          processGetRequest();
        });

        return;
    }

    writeHeader("Content-Length", std::to_string(source->size()));
    writeHeader("Content-Type", requestInfo_.mimeType);
    flushResponseMetaInfo();
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncReadFile, Qt::QueuedConnection);
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncWriteToSocket, Qt::QueuedConnection);
    QObject::connect(socket_, &QTcpSocket::bytesWritten, this, &HttpHandler::countSentBytes);
    asyncExecutor_.start();
}

void HttpHandler::asyncWriteToSocket() {


    auto[bytesToEnd, bytesFromBegin] = outputBuffer_.totalBytesAvailableToRead();

    if (bytesToEnd + bytesFromBegin == 0) {
        return;
    }
    outputBuffer_.readFromBuffer(socket_, bytesToEnd + bytesFromBegin);
}

void HttpHandler::asyncReadFile() {
    if (source->atEnd()) {
        return;
    }


    auto[bytesToEnd, bytesFromBegin] = outputBuffer_.totalBytesAvailableToWrite();

    if (bytesToEnd + bytesFromBegin == 0) {
        return;
    }

    bytesWritten_ += outputBuffer_.writeToBuffer(source, bytesToEnd + bytesFromBegin);

    qDebug() << "written" << bytesWritten_;

    if (source->atEnd()) {
        qDebug() << "all bytes in buffer";
        source->close();
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
    qDebug() << "sent " << sentChunk << "total" << bytesSent_ << "written" << bytesWritten_;
    if (allBytesInBuffer && bytesSent_ == bytesWritten_) {
        qDebug() << "done: bytes written " << bytesWritten_;
        qDebug() << "done: bytes sent " << bytesSent_;
        asyncExecutor_.stop();
        closeConnection();
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
    QObject::connect(&asyncExecutor_, &QTimer::timeout, this, &HttpHandler::asyncWriteToSocket, Qt::QueuedConnection);
    QObject::connect(socket_, &QTcpSocket::bytesWritten, this, &HttpHandler::countSentBytes, Qt::QueuedConnection);
    asyncExecutor_.start();
}

void HttpHandler::sendDefaultResponse() {
    flushResponseMetaInfo();
    asyncFlushBuffer();
}






