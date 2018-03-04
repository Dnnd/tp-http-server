
#include "HandlerHolder.h"

void HandlerHolder::registerConnection(int socketDescriptor) {

    auto *socket = new QTcpSocket{this};
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << socket->error();
    }
    Handler *handler = createHandler(socket, this);

    registerHandler(handler);
}

void HandlerHolder::registerHandler(Handler *handler) {

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
