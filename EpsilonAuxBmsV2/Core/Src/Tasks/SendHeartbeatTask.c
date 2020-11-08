#include "SendHeartbeatTask.h"

void sendHeartbeatTask(void* arg)
{
    CanTxGatekeeperQueueData canQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        sendHeartbeat(&canQueueData, &prevWakeTime);
    }
}

// Creates a CAN message for reporting the Aux BMS Heartbeat
void sendHeartbeat(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime)
{
    canQueueData->canTxHeader = baseCanTxHdr;
    canQueueData->canTxHeader.StdId = HEARTBEAT_STDID;
    canQueueData->canTxHeader.DLC = 1;
    canQueueData->data[0] = 1;
    osMessageQueuePut(canTxGatekeeperQueue, canQueueData, 0, 0);
    *prevWakeTime += SEND_HEARTBEAT_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}
