#ifndef TP_HTTP_SERVER_CONCURRENTTCPSERVER_H
#define TP_HTTP_SERVER_CONCURRENTTCPSERVER_H


#include <QtNetwork/QTcpServer>
#include <QThreadPool>
#include "SocketHandlerHolder.h"
#include "HolderController.h"

class ConcurrentTcpServer : public QTcpServer {
Q_OBJECT
public:
    explicit ConcurrentTcpServer(unsigned threadsLimit, QObject *parent = nullptr);
    void incomingConnection(qintptr socketDescriptor) override;
    ~ConcurrentTcpServer() override;
protected:
    HolderController* scheduleHolder();
private:
    std::vector<QThread> threadPool_;
    std::vector<HolderController*> holderControllers_;
    std::size_t topThread{0};
};



#endif //TP_HTTP_SERVER_CONCURRENTTCPSERVER_H
