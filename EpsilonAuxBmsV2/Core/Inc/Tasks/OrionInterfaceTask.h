#pragma once
#include "cmsis_os.h"
#include "ContactorGatekeeper.h"
#include "OrionInterfaceQueueData.h"
#include "AuxStatus.h"
#include "AuxTrip.h"
#include "DisconnectContactors.h"

void orionInterfaceTask(void* arg);
void orionInterface(void);
void processCanMessage(OrionCanInfo canInfo);
void processGpioMessage(uint16_t gpioNum);

extern osMessageQueueId_t orionInterfaceQueue;
extern AuxStatus auxStatus;
extern AuxTrip auxTrip;
