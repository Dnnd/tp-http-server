#ifndef TP_HTTP_SERVER_HTTPSERVER_H
#define TP_HTTP_SERVER_HTTPSERVER_H


#include <QtCore/QArgument>
#include <QtNetwork/QTcpServer>
#include "../Config/Config.h"
#include "ConcurrentTcpServer/ConcurrentTcpServer.h"

class HttpServer : public QObject {
Q_OBJECT
public:
    explicit HttpServer(Config config, QObject *parent = nullptr);
    bool run();
private:
    Config config_;
    ConcurrentTcpServer *tcp_server_{nullptr};
};


#endif //TP_HTTP_SERVER_HTTPSERVER_H
