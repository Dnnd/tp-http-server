#include "Handler.h"

Handler::Handler(QTcpSocket *socket, QObject *parent)
  : QObject{parent},
    socket_{socket} {
    QObject::connect(socket_, &QTcpSocket::readyRead, this, &Handler::handleNewDataSlot);
    QObject::connect(socket_, &QTcpSocket::disconnected, this, &Handler::handleDisconnectSlot);
    QObject::connect(socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
                     this,
                     &Handler::handleErrorSlot);


}

void Handler::handleNewDataSlot() {
    handleNewData();
}

void Handler::handleDisconnectSlot() {
    handleDisconnect();
}

void Handler::handleErrorSlot(QTcpSocket::SocketError error) {
    handleError(error);
}
