#ifndef TP_HTTP_SERVER_SOCKETHANDLERHOLDER
#define TP_HTTP_SERVER_SOCKETHANDLERHOLDER


#include <QtCore/QRunnable>
#include <QTcpSocket>
#include <unordered_set>
#include "../ConcurrentTcpServer/Handler.h"
#include "../ConcurrentTcpServer/HandlerHolder.h"


class HttpHandlerHolder : public HandlerHolder {

public:
    explicit HttpHandlerHolder(QString documentRoot);
    Handler *createHandler(QTcpSocket *socket, QObject *parent) const override;
private:
    QString documentRoot_;
};


#endif //TP_HTTP_SERVER_SOCKETHANDLERHOLDER
