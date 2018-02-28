#include <iostream>
#include <QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include "Config/Config.h"
#include "Server/HttpServer/HttpServer.h"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QLocale::setDefault(QLocale::English);

    Config conf;
    conf.readConfig("/etc/httpd.conf");

    HttpServer server{std::move(conf), &app};

    if (server.run()) {
        qInfo() << "Server started";
        return app.exec();
    } else {
        app.exit(1);
    }

}