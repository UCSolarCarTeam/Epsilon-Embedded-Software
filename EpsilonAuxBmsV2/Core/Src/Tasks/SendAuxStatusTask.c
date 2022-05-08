#include "SendAuxStatusTask.h"

void sendAuxStatusTask(void* arg)
{
    CanTxGatekeeperQueueData canQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };
    uint32_t prevWakeTime = xTaskGetTickCount();

    for (;;)
    {
        sendAuxStatus(&canQueueData, &prevWakeTime);
    }
}

/*
Executes at 10Hz.
Reads the auxStatus variable and creates a CAN message for reporting Aux Status.
Can only read auxStatus variable after acquiring auxStatusOrionInterfaceMutex,  auxStatusReadAuxVoltageMutex, auxStatusContactorStatusUpdateMutex.
Puts message on canTxGatekeeperQueue.

Look at comms protocol for CAN data assignment
*/
void sendAuxStatus(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime)
{
    if (osMutexAcquire(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT) == osOK)
    {
        if (osMutexAcquire(auxStatusReadAuxVoltageMutex, MUTEX_TIMEOUT) == osOK)
        {
            if (osMutexAcquire(auxStatusContactorStatusUpdateMutex, MUTEX_TIMEOUT) == osOK)
            {
                canQueueData->canTxHeader = BASE_CAN_TX_HDR;
                canQueueData->canTxHeader.StdId = AUX_STATUS_STDID;
                canQueueData->canTxHeader.DLC = 3;
                canQueueData->data[0] = auxStatus.commonContactorState |
                                        (auxStatus.chargeContactorState << 1) |
                                        (auxStatus.dischargeContactorState << 2) |
                                        (auxStatus.auxVoltage << 3);
                canQueueData->data[1] = auxStatus.strobeBmsLight |
                                        (auxStatus.allowCharge << 1) |
                                        (auxStatus.highVoltageEnableState << 2) |
                                        (auxStatus.allowDischarge << 3) |
                                        (auxStatus.orionCanReceivedRecently << 4) |
                                        (auxStatus.chargeContactorError << 5) |
                                        (auxStatus.dischargeContactorError << 6) |
                                        (auxStatus.commonContactorError << 7);
                canQueueData->data[2] = auxStatus.dischargeShouldTrip |
                                        (auxStatus.chargeShouldTrip << 1) |
                                        (auxStatus.chargeOpenButShouldBeClosed << 2) |
                                        (auxStatus.dischargeOpenButShouldBeClosed << 3);
                osMessageQueuePut(canTxGatekeeperQueue, canQueueData, 0, TASK_QUEUE_PUT_TIMEOUT);
                osMutexRelease(auxStatusContactorStatusUpdateMutex);
            }

            osMutexRelease(auxStatusReadAuxVoltageMutex);
        }

        osMutexRelease(auxStatusOrionInterfaceMutex);
    }

    *prevWakeTime += SEND_AUX_STATUS_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}
