#include <QtNetwork/QTcpSocket>
#include "HttpHandlerHolder.h"
#include "../HttpHandler/HttpHandler.h"

Handler *HttpHandlerHolder::createHandler(QTcpSocket *socket, QObject *parent) const {
    Handler *handler = new HttpHandler(documentRoot_, socket, parent);
    return handler;
}

HttpHandlerHolder::HttpHandlerHolder(QString documentRoot)
        : documentRoot_{std::move(documentRoot)} {

}
