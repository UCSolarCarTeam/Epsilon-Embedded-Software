#pragma once
#include "ContactorGatekeeper.h"

extern osThreadId_t dischargeContactorGatekeeperTaskHandle;
extern AuxBmsContactorState auxBmsContactorState;

void dischargeContactorGatekeeperTask(void* arg);
void closeDischargeContactor();
void openDischargeContactor();
void dischargeContactorGatekeeper();
