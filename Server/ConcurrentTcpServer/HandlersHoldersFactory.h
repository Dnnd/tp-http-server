#ifndef TP_HTTP_SERVER_HANDLERFACTORY_H
#define TP_HTTP_SERVER_HANDLERFACTORY_H


#include "HandlerHolder.h"

class HandlersHoldersFactory {

public:

  virtual HandlerHolder *createHandlerHolder() const = 0;

  virtual ~HandlersHoldersFactory() = default;
};


#endif //TP_HTTP_SERVER_HANDLERFACTORY_H
