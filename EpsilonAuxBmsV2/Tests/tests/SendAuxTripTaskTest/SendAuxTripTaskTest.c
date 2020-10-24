#include "SendAuxTripTaskTest.h"
#include "unity.h"
#include "SendAuxTripTask.h"
#include "Mockcmsis_os2.h"


osMutexId_t auxTripMutex;

const CAN_TxHeaderTypeDef baseCanTxHdr;
osMessageQueueId_t canTxGatekeeperQueue;

AuxTrip auxTrip = {
    .chargeTripDueToHighCellVoltage = 1, .chargeTripDueToHighTemperatureAndCurrent = 0,
    .chargeTripDueToPackCurrent = 0, .dischargeTripDueToLowCellVoltage = 1,
    .dischargeTripDueToHighTemperatureAndCurrent = 0, .dischargeTripDueToPackCurrent = 1,
    .protectionTrip = 0
};

CanTxGatekeeperQueueData canQueueData;

CanTxGatekeeperQueueData actualCanQueueData;

void checkCanQueueData ()
{
    actualCanQueueData.data[0] = 0b0101001;

    uint32_t prevWakeTime = 0;

    osMutexAcquire_ExpectAndReturn(auxTripMutex, 100, osOK);
    osMessageQueuePut_ExpectAndReturn(canTxGatekeeperQueue, &canQueueData, 0, 0,0);
    osMutexRelease_ExpectAndReturn(auxTripMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + SEND_AUX_TRIP_TASK_FREQ, osOK);

    checkCanQueueData( &canQueueData, &prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(canQueueData.data[0], actualCanQueueData.data[0],"Data bits at index 0 aren't equal");


}



int runSendAuxTripTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(checkCanQueueData);
    

    return UNITY_END();
}