#include "SendAuxStatusTask.h"

void sendAuxStatusTask(void* arg)
{
    CanTxGatekeeperQueueData canQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        sendAuxStatus(&canQueueData, &prevWakeTime);
    }
}

/*
Executes at 10Hz.
Reads the auxStatus variable and creates a CAN message for reporting Aux Status.
Can only read auxStatus variable after acquiring auxStatusOrionInterfaceMutex,  auxStatusReadAuxVoltageMuteauxStatusContactorStatusUpdateMutex.
Puts message on canTxGatekeeperQueue.

Look at comms protocol for CAN data assignment
*/
void sendAuxStatus(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime)
{
    if (osMutexAcquire(auxStatusOrionInterfaceMutex, 100) == osOK)
    {
        if (osMutexAcquire(auxStatusReadAuxVoltageMutex, 100) == osOK)
        {
            if (osMutexAcquire(auxStatusContactorStatusUpdateMutex, 100) == osOK)
            {
                canQueueData->canTxHeader = baseCanTxHdr;
                canQueueData->canTxHeader.StdId = AUX_STATUS_STDID;
                canQueueData->canTxHeader.DLC = 3;
                canQueueData->data[0] = auxStatus.commonContactorState |
                                        (auxStatus.chargeContactorState << 1) |
                                        (auxStatus.dischargeContactorState << 2) |
                                        (auxStatus.auxVoltage << 3);
                canQueueData->data[1] = auxStatus.highVoltageEnableState |
                                        (auxStatus.strobeBmsLight << 1) |
                                        (auxStatus.allowCharge << 2) |
                                        (auxStatus.allowDischarge << 3) |
                                        (auxStatus.orionCanReceivedRecently << 4) |
                                        (auxStatus.chargeContactorError << 5) |
                                        (auxStatus.dischargeContactorError << 6) |
                                        (auxStatus.commonContactorError << 7);
                canQueueData->data[2] = auxStatus.dischargeShouldTrip |
                                        (auxStatus.chargeShouldTrip << 1) |
                                        (auxStatus.chargeOpenButShouldBeClosed << 2) |
                                        (auxStatus.dischargeOpenButShouldBeClosed << 3);
                osMessageQueuePut(canTxGatekeeperQueue, canQueueData, 0, 0);
                osMutexRelease(auxStatusContactorStatusUpdateMutex);
            }

            osMutexRelease(auxStatusReadAuxVoltageMutex);
        }

        osMutexRelease(auxStatusOrionInterfaceMutex);
    }

    *prevWakeTime += SEND_AUX_STATUS_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}

