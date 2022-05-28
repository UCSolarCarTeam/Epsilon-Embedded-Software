#pragma once
#include "ContactorGatekeeper.h"
#include "AuxStatus.h"
#include "AuxTrip.h"
#include "DisconnectContactors.h"

extern osThreadId_t dischargeContactorGatekeeperTaskHandle;
extern AuxBmsContactorState auxBmsContactorState;
extern AuxTrip auxTrip;
extern AuxStatus auxStatus;

void dischargeContactorGatekeeperTask(void* arg);
void closeDischargeContactor();
void openDischargeContactor();
void dischargeContactorGatekeeper();
