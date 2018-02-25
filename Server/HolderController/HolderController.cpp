#include "HolderController.h"

void HolderController::scheduleConnectionRegistration(int socketDescriptor) {
    emit newDescriptor(socketDescriptor);

}
