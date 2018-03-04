#ifndef TP_HTTP_SERVER_HTTPREQUESTINFO_H
#define TP_HTTP_SERVER_HTTPREQUESTINFO_H

#include <QtCore/QFile>
#include "defs.h"

struct HttpRequestInfo {
  QString file;
  QString mimeType;
  std::size_t size;
  HttpMethod method{HttpMethod::GET};
  QString version;
};

#endif //TP_HTTP_SERVER_HTTPREQUESTINFO_H
