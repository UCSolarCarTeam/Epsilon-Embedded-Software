#pragma once
#include "SendCan.h"
void sendHeartbeatTask(void* arg);
void sendHeartbeat(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime);

static const uint32_t SEND_HEARTBEAT_TASK_FREQ = 1000; // 1s (1Hz)
static const uint32_t HEARTBEAT_STDID = 0x720;
