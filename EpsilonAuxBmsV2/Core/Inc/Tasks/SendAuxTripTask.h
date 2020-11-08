#pragma once
#include "SendCan.h"
#include "AuxTrip.h"
void sendAuxTripTask(void* arg);
void sendAuxTrip(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime);

extern osMutexId_t auxTripMutex;
extern AuxTrip auxTrip;

static const uint32_t SEND_AUX_TRIP_TASK_FREQ = 100; // 100ms (10Hz)
static const uint32_t AUX_TRIP_STDID = 0x722;
