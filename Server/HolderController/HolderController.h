#ifndef TP_HTTP_SERVER_HOLDERCONTROLLER_H
#define TP_HTTP_SERVER_HOLDERCONTROLLER_H


#include <QtCore/QObject>

class HolderController : public QObject {
Q_OBJECT
public:
  void scheduleConnectionRegistration(int socketDescriptor);

signals:

  void newDescriptor(int socketDescriptor);
};


#endif //TP_HTTP_SERVER_HOLDERCONTROLLER_H
