
#include <QtNetwork/QTcpSocket>
#include "SocketHandlerHolder.h"
#include <iostream>

void SocketHandlerHolder::registerConnection(int socketDescriptor) {

    auto *socket = new QTcpSocket{this};

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << socket->error();
    }

    QByteArray buffer{};

    QObject::connect(socket, &QTcpSocket::readyRead, [socket_ = socket, buffer_ = std::move(buffer)]() mutable {
        buffer_.append(socket_->readAll());
        qDebug() << buffer_;
    });

    QObject::connect(socket, &QTcpSocket::disconnected, [socket_ = socket]() mutable {
        qDebug() << "Deletion";
        socket_->deleteLater();
    });

    QObject::connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
    (&QAbstractSocket::error), [socket](QAbstractSocket::SocketError) {
        qDebug() << "Error " << socket->errorString();
        socket->deleteLater();
    });

};
