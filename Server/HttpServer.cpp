#include <QtCore/QSettings>
#include "HttpServer.h"


HttpServer::HttpServer(Config config, QObject *parent)
        : QObject{parent},
          config_{std::move(config)} {
    if (!config_.isValid()) {
        throw std::runtime_error("invalid config");
    }
}


bool HttpServer::run() {
    tcp_server_ = new ConcurrentTcpServer{config_.getCpuLimit(), this};
    return tcp_server_->listen(QHostAddress::LocalHost, config_.getPort());
}

