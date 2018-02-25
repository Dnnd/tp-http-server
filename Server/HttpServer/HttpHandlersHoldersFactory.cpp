#include "HttpHandlersHoldersFactory.h"
#include "../HttpHandlerHolder/HttpHandlerHolder.h"

HandlerHolder *HttpHandlersHoldersFactory::createHandlerHolder() const {
    HttpHandlerHolder *holder = new HttpHandlerHolder{documentRoot_};
    return holder;
}

HttpHandlersHoldersFactory::HttpHandlersHoldersFactory(const QString &documentRoot)
    :documentRoot_{documentRoot}{

}

