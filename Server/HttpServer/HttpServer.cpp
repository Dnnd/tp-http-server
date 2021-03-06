#include <QtCore/QSettings>
#include "HttpServer.h"
#include "HttpHandlersHoldersFactory.h"


HttpServer::HttpServer(Config config, QObject *parent)
  : QObject{parent},
    config_{std::move(config)} {
}


bool HttpServer::run() {

    tcp_server_ = new ConcurrentTcpServer{config_.getCpuLimit(),
                                          std::make_unique<HttpHandlersHoldersFactory>(config_.getDocumentRoot()),
                                          this};
    return tcp_server_->listen(QHostAddress::Any, config_.getPort());
}

uint16_t HttpServer::port() const {
    return config_.getPort();
}

