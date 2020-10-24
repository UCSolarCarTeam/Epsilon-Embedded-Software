#include "SendAuxStatusTaskTest.h"
#include "unity.h"
#include "SendAuxStatusTask.h"
#include "Mockcmsis_os2.h"

osMutexId_t auxStatusOrionInterfaceMutex;
osMutexId_t auxStatusReadAuxVoltageMutex;
osMutexId_t auxStatusContactorStatusUpdateMutex;

const CAN_TxHeaderTypeDef baseCanTxHdr;
osMessageQueueId_t canTxGatekeeperQueue;

AuxStatus auxStatus = { .commonContactorState = 0b1, .chargeContactorState = 0,
                        .dischargeContactorState = 1, .auxVoltage = 0b11010,
                        .highVoltageEnableState = 0, .strobeBmsLight = 1,
                        .allowCharge = 0, .allowDischarge = 1,
                        .orionCanReceivedRecently = 0, .chargeContactorError = 1,
                        .dischargeContactorError = 0, .commonContactorError = 1,
                        .dischargeShouldTrip = 0, .chargeShouldTrip = 1,
                        .chargeOpenButShouldBeClosed = 0, 
                        .dischargeOpenButShouldBeClosed = 1 };

CanTxGatekeeperQueueData canQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };

CanTxGatekeeperQueueData expectedCanQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };


void checkCanQueueDataTest()
{
    

    expectedCanQueueData.data[0] = 0b11010101;
    expectedCanQueueData.data[1] = 0b10101010;
    expectedCanQueueData.data[2] = 0b1010;
    
    expectedCanQueueData.canTxHeader.StdId = AUX_STATUS_STDID;
    
    uint32_t prevWakeTime = 0;
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, 100, osOK);
    osMutexAcquire_ExpectAndReturn(auxStatusReadAuxVoltageMutex, 100, osOK);
    osMutexAcquire_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 100, osOK);
    osMessageQueuePut_ExpectAndReturn(canTxGatekeeperQueue, &canQueueData, 0, 0,0);
    osMutexRelease_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 0);
    osMutexRelease_ExpectAndReturn(auxStatusReadAuxVoltageMutex, 0);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + SEND_AUX_STATUS_TASK_FREQ, osOK);
    sendAuxStatus(&canQueueData, &prevWakeTime);

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(canQueueData.data, expectedCanQueueData.data, 8, "Data bits aren't equal");



}

int runSendAuxStatusTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(checkCanQueueDataTest);
    

    return UNITY_END();
}



