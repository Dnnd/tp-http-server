#include <iostream>
#include <QCoreApplication>
#include "Config/Config.h"
#include "Server/HttpServer.h"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    Config conf;
    conf.readConfig("/etc/httpd.conf");
    HttpServer server{std::move(conf), &app};
    server.run();
    return app.exec();
}