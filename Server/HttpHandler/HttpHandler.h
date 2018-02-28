#ifndef TP_HTTP_SERVER_HTTPHANDLER_H
#define TP_HTTP_SERVER_HTTPHANDLER_H


#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QMimeDatabase>
#include "../ConcurrentTcpServer/Handler.h"
#include "../HttpParser/HttpParser.h"
#include "../../IOBuffer/IOBuffer.h"

class HttpHandler : public Handler {
Q_OBJECT
public:
  explicit HttpHandler(const QString &documentRoot, QTcpSocket *socket, QObject *parent = nullptr);

  void handleNewData() override;

  void handleDisconnect() override;

  void handleError(QTcpSocket::SocketError error) override;

  void closeConnection();


  void processGetRequest();

  void writeDefaultHeaders();

  void writeResponseString(HttpStatusCode code);

  void writeHeader(std::string headerName, std::string headerValue);

  void writeHeader(std::string headerName, const QString &headerValue);

  static const int CHUNK_SIZE = 64 * 1024;

protected slots:

  void asyncWriteToSocket();

  void asyncReadFile();

  void countSentBytes(qint64 sentChunk);

private:

  QByteArray buffer_;

  QTimer asyncExecutor_;

  IOBuffer outputBuffer_{CHUNK_SIZE * 2};
  QByteArray responseMetaInfoBuffer_;

  std::size_t bytesWritten_{0};
  std::size_t bytesSent_{0};

  HttpParser parser_;
  HttpRequestInfo requestInfo_;
  QFile *source;

  static const QString dateformat;
  bool allBytesInBuffer{false};

  void processHeadRequest();

  void flushResponseMetaInfo();
};


#endif //TP_HTTP_SERVER_HTTPHANDLER_H
