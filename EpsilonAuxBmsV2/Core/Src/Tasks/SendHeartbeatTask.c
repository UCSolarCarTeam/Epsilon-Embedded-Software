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
    canQueueData->canTxHeader = BASE_CAN_TX_HDR;
    canQueueData->canTxHeader.StdId = HEARTBEAT_STDID;
    canQueueData->canTxHeader.DLC = 1;
    canQueueData->data[0] = 1;
    osMessageQueuePut(canTxGatekeeperQueue, canQueueData, 0, TASK_QUEUE_PUT_TIMEOUT);
    *prevWakeTime += SEND_HEARTBEAT_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}
