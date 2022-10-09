#pragma once
#include "ContactorGatekeeper.h"
#include "AuxTrip.h"

extern osThreadId_t dischargeContactorGatekeeperTaskHandle;
extern AuxBmsContactorState auxBmsContactorState;
extern AuxTrip auxTrip;
extern osMutexId_t auxTripMutex;

void dischargeContactorGatekeeperTask(void* arg);
void closeDischargeContactor();
void openDischargeContactor();
void dischargeContactorGatekeeper();

extern traceString dischargeStateTrace;
