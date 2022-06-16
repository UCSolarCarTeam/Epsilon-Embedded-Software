#pragma once
#include "cmsis_os.h"
#include "ContactorGatekeeper.h"
#include "OrionCanInfo.h"
#include "AuxStatus.h"
#include "AuxTrip.h"
#include "DisconnectContactors.h"
#include "Trip.h"
#include "ContactorGatekeeper.h"

void orionInterfaceTask(void* arg);
void orionInterface(OrionCanInfo* message);
void updateGlobalAuxTrip(AuxTrip* auxTripToRead);
void updateAuxStatus(AuxStatus* auxStatusToRead);
void updateAllowChargeAndAllowDischarge(OrionCanInfo* message, AuxStatus* auxStatusToUpdate);
uint8_t checkIfOrionGood(OrionCanInfo* message, uint32_t* startUpCounter);

AuxStatus localAuxStatus;
AuxTrip localAuxTrip;

extern osMessageQueueId_t orionInterfaceQueue;
extern AuxStatus auxStatus;
extern AuxTrip auxTrip;
extern AuxBmsContactorState auxBmsContactorState;
extern osMutexId_t auxStatusOrionInterfaceMutex;
extern osMutexId_t auxTripMutex;
extern uint32_t manualChargeTrip;

static const uint32_t ORION_QUEUE_TIMEOUT = 400; // 400ms. We expect CAN messages from Orion every 200ms, so if we miss 2 messages, we can timeout the wait
