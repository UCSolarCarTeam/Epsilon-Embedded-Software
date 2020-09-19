#pragma once
#include "cmsis_os.h"
#include "ContactorGatekeeper.h"
#include "OrionCanInfo.h"
#include "AuxStatus.h"
#include "AuxTrip.h"
#include "DisconnectContactors.h"
#include "Trip.h"

void orionInterfaceTask(void* arg);
void orionInterface(void);
void updateAuxStatus(AuxStatus* auxStatusToRead);
void checkCellVoltage(OrionCanInfo* message, AuxStatus* auxStatusToUpdate);


extern osMessageQueueId_t orionInterfaceQueue;
extern AuxStatus auxStatus;
extern AuxTrip auxTrip;
extern AuxBmsContactorState auxBmsContactorState;
extern osMutexId_t auxStatusOrionInterfaceMutex;
extern osMutexId_t auxTripMutex;
