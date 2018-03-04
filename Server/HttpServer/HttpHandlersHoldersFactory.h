#ifndef TP_HTTP_SERVER_HTTPHANDLERSHOLDERSFACTORY_H
#define TP_HTTP_SERVER_HTTPHANDLERSHOLDERSFACTORY_H


#include <QtCore/QDir>
#include "../ConcurrentTcpServer/HandlersHoldersFactory.h"

class HttpHandlersHoldersFactory : public HandlersHoldersFactory {
public:
  explicit HttpHandlersHoldersFactory(const QString &documentRoot);

  HandlerHolder *createHandlerHolder() const override;

private:
  QString documentRoot_;
};


#endif //TP_HTTP_SERVER_HTTPHANDLERSHOLDERSFACTORY_H
