#include <QtCore/QThreadPool>
#include "ConcurrentTcpServer.h"

ConcurrentTcpServer::ConcurrentTcpServer(unsigned threadsLimit, QObject *parent)
        : QTcpServer{parent},
          threadPool_{threadsLimit},
          topThread{0} {

    for (auto &&thread : threadPool_) {
        auto *holder = new SocketHandlerHolder{};
        auto *controller = new HolderController{};
        QObject::connect(controller, &HolderController::newDescriptor,
                         holder, &SocketHandlerHolder::registerConnection);
        QObject::connect(&thread, &QThread::finished, holder, &QObject::deleteLater);
        holder->moveToThread(&thread);
        holderControllers_.push_back(controller);
        thread.start();
    }
}

void ConcurrentTcpServer::incomingConnection(qintptr socketDescriptor) {
    auto *holder = scheduleHolder();
    holder->sheduleConnectionRegistration(static_cast<int>(socketDescriptor));
}

HolderController *ConcurrentTcpServer::scheduleHolder() {
    auto current = topThread++;
    qDebug() << current << " " << holderControllers_.size();

    current %= static_cast<std::size_t>(holderControllers_.size());
    return holderControllers_[current];
}

ConcurrentTcpServer::~ConcurrentTcpServer() {
    for (auto &&controller: holderControllers_) {
        controller->deleteLater();
    }
    for (auto &&thread: threadPool_) {
        thread.quit();
        thread.wait();
    }

}


