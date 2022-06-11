#pragma once
#include "cmsis_os.h"
#include "ContactorEventFlags.h"
#include "ContactorState.h"

extern osEventFlagsId_t contactorControlEventBits;
extern AuxBmsContactorState auxBmsContactorState;
extern osThreadId_t commonContactorGatekeeperTaskHandle;
extern osThreadId_t chargeContactorGatekeeperTaskHandle;
extern osThreadId_t dischargeContactorGatekeeperTaskHandle;

void disconnectContactors();
