#pragma once
#include "ContactorGatekeeper.h"
#include "AuxTrip.h"

extern osThreadId_t chargeContactorGatekeeperTaskHandle;
extern AuxTrip auxTrip;
extern osMutexId_t auxTripMutex;

void closeChargeContactor();
void openChargeContactor();
void chargeContactorGatekeeperTask(void* arg);
void chargeContactorGatekeeper();

extern traceString chargeStateTrace;
