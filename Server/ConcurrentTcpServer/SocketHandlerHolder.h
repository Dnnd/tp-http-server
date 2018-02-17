#ifndef TP_HTTP_SERVER_SOCKETHANDLERHOLDER
#define TP_HTTP_SERVER_SOCKETHANDLERHOLDER


#include <QtCore/QRunnable>
#include <QTcpSocket>

class SocketHandlerHolder : public QObject {
Q_OBJECT

public slots:

    void registerConnection(int socketDescriptor);

};


#endif //TP_HTTP_SERVER_SOCKETHANDLERHOLDER
