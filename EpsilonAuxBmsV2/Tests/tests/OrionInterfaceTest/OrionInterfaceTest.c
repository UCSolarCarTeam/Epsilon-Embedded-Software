#include "OrionInterfaceTest.h"

OrionCanInfo message;
osMessageQueueId_t orionInterfaceQueue;
AuxStatus auxStatus;
AuxTrip auxTrip;
AuxBmsContactorState auxBmsContactorState;
osMutexId_t auxStatusOrionInterfaceMutex;
osMutexId_t auxTripMutex;
osEventFlagsId_t contactorControlEventBits;
osThreadId_t commonContactorGatekeeperTaskHandle;
osThreadId_t chargeContactorGatekeeperTaskHandle;
osThreadId_t dischargeContactorGatekeeperTaskHandle;

void runOrionInterfaceTests()
{
    // RUN_TEST(test_onlyUpdateAllowCharge);
    // RUN_TEST(test_onlyUpdateAllowDischarge);
    RUN_TEST(test_osMessageQueueGetTimeout);
    RUN_TEST(test_shouldDisconnectContactors);
}

// void test_onlyUpdateAllowCharge() {
//     AuxStatus auxStatusToUpdate;
//     auxStatusToUpdate.allowCharge = 1;
//     message.highCellVoltage = 42000;
//     updateAllowChargeAndAllowDischarge(&message, &auxStatusToUpdate);
//     TEST_ASSERT_EQUAL_MESSAGE(0, auxStatusToUpdate.allowCharge, "auxStatusToUpdate.allowCharge failed to update");
// }

// void test_onlyUpdateAllowDischarge() {
//     AuxStatus auxStatusToUpdate;
//     auxStatusToUpdate.allowDischarge = 1;
//     message.lowCellVoltage = 25000;
//     updateAllowChargeAndAllowDischarge(&message, &auxStatusToUpdate);
//     TEST_ASSERT_EQUAL_MESSAGE(0, auxStatusToUpdate.allowDischarge, "auxStatusToUpdate.allowDischarge failed to update");
// }

void test_osMessageQueueGetTimeout() {
    AuxStatus expectedAuxStatus = {.orionCanReceivedRecently = 0};
    
    osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, ORION_QUEUE_TIMEOUT, osErrorTimeout);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, HAL_ERROR);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, HAL_ERROR);
    osMutexAcquire_ExpectAndReturn(auxTripMutex, MUTEX_TIMEOUT, osError);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);
    
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.orionCanReceivedRecently, auxStatus.orionCanReceivedRecently, "auxStatus.orionCanReceivedRecently failed to update");
}

void test_shouldDisconnectContactors() {
    AuxStatus expectedAuxStatus = {
        .orionCanReceivedRecently = 1,
        .allowCharge = 0,
        .allowDischarge = 0,
        .strobeBmsLight = 1
    };

    osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, ORION_QUEUE_TIMEOUT, osOK);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, HAL_ERROR);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, HAL_ERROR);
    osMutexAcquire_ExpectAndReturn(auxTripMutex, MUTEX_TIMEOUT, osError);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);

    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.orionCanReceivedRecently, auxStatus.orionCanReceivedRecently, "auxStatus.orionCanReceivedRecently failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowCharge, auxStatus.allowCharge, "auxStatus.allowCharge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowDischarge, auxStatus.allowDischarge, "auxStatus.allowDischarge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.strobeBmsLight, auxStatus.strobeBmsLight, "auxStatus.strobeBmsLight failed to update");
}
