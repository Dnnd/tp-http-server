
#include "HandlerHolder.h"
void HandlerHolder::registerConnection(int socketDescriptor) {

    auto *socket = new QTcpSocket{this};

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << socket->error();
    }
    Handler *handler = createHandler(socket, this);
    qDebug() << "handler created";
    registerHandler(handler);
}

void HandlerHolder::registerHandler(Handler *handler) {
    qDebug() << "handler registered";
    QObject::connect(handler, &Handler::finish, this, &HandlerHolder::removeHandler);
    handlers_.insert(handler);
}

void HandlerHolder::removeHandler(Handler *handler) {
    auto handlerNode = handlers_.extract(handler);
    if (handlerNode.empty()) {
        return;
    }
    handlerNode.value()->deleteLater();
};
