#ifndef TP_HTTP_SERVER_CONCURRENTTCPSERVER_H
#define TP_HTTP_SERVER_CONCURRENTTCPSERVER_H


#include <QtNetwork/QTcpServer>
#include <QThreadPool>
#include "../HttpHandlerHolder/HttpHandlerHolder.h"
#include "../HolderController/HolderController.h"
#include "HandlersHoldersFactory.h"

class ConcurrentTcpServer : public QTcpServer {
Q_OBJECT
public:
    explicit ConcurrentTcpServer(unsigned threadsLimit, std::unique_ptr<HandlersHoldersFactory> holdersFactory, QObject *parent = nullptr);
    void incomingConnection(qintptr socketDescriptor) override;
    ~ConcurrentTcpServer() override;
protected:
    HolderController* scheduleHolder();
private:
    std::vector<QThread> threadPool_;
    std::vector<HolderController*> holderControllers_;
    std::size_t topThread{0};
    std::unique_ptr<HandlersHoldersFactory> holdersFactory_;
};



#endif //TP_HTTP_SERVER_CONCURRENTTCPSERVER_H
