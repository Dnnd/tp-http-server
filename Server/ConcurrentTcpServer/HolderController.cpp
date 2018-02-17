#include "HolderController.h"

void HolderController::sheduleConnectionRegistration(int socketDescriptor) {
    emit newDescriptor(socketDescriptor);

}
