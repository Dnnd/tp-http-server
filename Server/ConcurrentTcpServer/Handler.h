#ifndef TP_HTTP_SERVER_HANDLER_H
#define TP_HTTP_SERVER_HANDLER_H


#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

class Handler : public QObject {
Q_OBJECT
public:
  explicit Handler(QTcpSocket *socket, QObject *parent = nullptr);

  virtual void handleNewData() = 0;

  virtual void handleDisconnect() = 0;

  virtual void handleError(QTcpSocket::SocketError error) = 0;

public slots:

  void handleNewDataSlot();

  void handleDisconnectSlot();

  void handleErrorSlot(QTcpSocket::SocketError error);

signals:

  void finish(Handler *handler);

protected:
  QTcpSocket *socket_;
};


#endif //TP_HTTP_SERVER_HANDLER_H
