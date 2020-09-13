#pragma once
#include "cmsis_os.h"
#include "ContactorGatekeeper.h"
#include "OrionCanInfo.h"
#include "AuxStatus.h"
#include "AuxTrip.h"
#include "DisconnectContactors.h"

void orionInterfaceTask(void* arg);
void orionInterface(void);

extern osMessageQueueId_t orionInterfaceQueue;
extern AuxStatus auxStatus;
extern AuxTrip auxTrip;
