#include "SendAuxTripTask.h"

void sendAuxTripTask(void* arg)
{
    CanTxGatekeeperQueueData canQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };
    uint32_t prevWakeTime = xTaskGetTickCount();

    for (;;)
    {
        sendAuxTrip(&canQueueData, &prevWakeTime);
    }
}

/*
Executes at 10Hz.
Reads the AuxTrip variable and creates a CAN message for reporting Aux Trip.
Can only read AuxTrip variable after acquiring auxTripMutex.
Puts message on canTxGatekeeperQueue.

Look at comms protocol for CAN data assignment
*/
void sendAuxTrip(CanTxGatekeeperQueueData* canQueueData, uint32_t* prevWakeTime)
{
    if (osMutexAcquire(auxTripMutex, MUTEX_TIMEOUT) == osOK)
    {
        canQueueData->canTxHeader = BASE_CAN_TX_HDR;
        canQueueData->canTxHeader.StdId = AUX_TRIP_STDID;
        canQueueData->canTxHeader.DLC = 2;
        canQueueData->data[0] = auxTrip.chargeTripDueToHighCellVoltage |
                                (auxTrip.chargeTripDueToHighTemperatureAndCurrent << 1) |
                                (auxTrip.chargeTripDueToPackCurrent << 2) |
                                (auxTrip.dischargeTripDueToLowCellVoltage << 3) |
                                (auxTrip.dischargeTripDueToHighTemperatureAndCurrent << 4) |
                                (auxTrip.dischargeTripDueToPackCurrent << 5) |
                                (auxTrip.protectionTrip << 6) |
                                (auxTrip.tripDueToOrionMessageTimeout << 7);
        osMessageQueuePut(canTxGatekeeperQueue, canQueueData, 0, TASK_QUEUE_PUT_TIMEOUT);
        osMutexRelease(auxTripMutex);
    }

    *prevWakeTime += SEND_AUX_TRIP_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}
