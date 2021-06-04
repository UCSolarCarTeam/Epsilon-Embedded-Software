#pragma once
#include "SendCan.h"
#include "AuxStatus.h"
void sendAuxStatusTask(void* arg);
void sendAuxStatus(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime);

extern osMutexId_t auxStatusOrionInterfaceMutex;
extern osMutexId_t auxStatusReadAuxVoltageMutex;
extern osMutexId_t auxStatusContactorStatusUpdateMutex;
extern AuxStatus auxStatus;

static const uint32_t SEND_AUX_STATUS_TASK_FREQ = 100; // 100ms (10Hz)
static const uint32_t AUX_STATUS_STDID = 0x721;
