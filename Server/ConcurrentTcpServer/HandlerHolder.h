#ifndef TP_HTTP_SERVER_HANDLERHOLDER_H
#define TP_HTTP_SERVER_HANDLERHOLDER_H


#include <QtCore/QObject>
#include <unordered_set>
#include "Handler.h"

class HandlerHolder : public QObject {
    Q_OBJECT
public:
    void registerHandler(Handler *handler);

    virtual Handler* createHandler(QTcpSocket *socket, QObject *parent) const = 0;

public slots:
    void registerConnection(int socketDescriptor);
    void removeHandler(Handler *handler);

private:
    std::unordered_set<Handler*> handlers_;
};


#endif //TP_HTTP_SERVER_HANDLERHOLDER_H
