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

// Because of localAuxTrip being a local variable, it is not possible to test
// trip conditions and contactor settings based on Orion CAN

void runOrionInterfaceTests()
{
    RUN_TEST(test_onlyUpdateAllowCharge);
    RUN_TEST(test_onlyUpdateAllowDischarge);
    RUN_TEST(test_osMessageQueueGetTimeout);
    RUN_TEST(test_shouldDisconnectContactors);
    RUN_TEST(test_shouldNotDisconnectContactorsChargeStateOpen);
    RUN_TEST(test_shouldNotDisconnectContactorsChargeStateClosed);
}

void test_onlyUpdateAllowCharge() {
    AuxStatus auxStatusToUpdate;
    auxStatusToUpdate.allowCharge = 1;
    message.highCellVoltage = 42000;
    updateAllowChargeAndAllowDischarge(&message, &auxStatusToUpdate);
    TEST_ASSERT_EQUAL_MESSAGE(0, auxStatusToUpdate.allowCharge, "auxStatusToUpdate.allowCharge failed to update");
}

void test_onlyUpdateAllowDischarge() {
    AuxStatus auxStatusToUpdate;
    auxStatusToUpdate.allowDischarge = 1;
    message.lowCellVoltage = 25000;
    updateAllowChargeAndAllowDischarge(&message, &auxStatusToUpdate);
    TEST_ASSERT_EQUAL_MESSAGE(0, auxStatusToUpdate.allowDischarge, "auxStatusToUpdate.allowDischarge failed to update");
}

// TODO: Find a better naming convetion for unit tests
void test_osMessageQueueGetTimeout() {
    AuxStatus expectedAuxStatus = {.orionCanReceivedRecently = 0};
    auxBmsContactorState.startupDone = 0;
    uint32_t contactorControlEventFlags = 0;
    
    osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, ORION_QUEUE_TIMEOUT, osErrorTimeout);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, 1);
    osMutexAcquire_ExpectAndReturn(auxTripMutex, MUTEX_TIMEOUT, osError);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, contactorControlEventFlags, 0);
    
    orionInterface(&message);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.orionCanReceivedRecently, auxStatus.orionCanReceivedRecently, "auxStatus.orionCanReceivedRecently failed to update");
}

void test_shouldDisconnectContactors() {
    AuxStatus expectedAuxStatus = {
        .orionCanReceivedRecently = 0,
        .allowCharge = 0,
        .allowDischarge = 0,
        .strobeBmsLight = 1
    };
    AuxBmsContactorState expectedAuxBmsContactorState = {.contactorsDisconnected = 1};

    osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, ORION_QUEUE_TIMEOUT, osErrorTimeout);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, 0);
    osMutexAcquire_ExpectAndReturn(auxTripMutex, MUTEX_TIMEOUT, osError);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);
    
    // From disconnectContactors() in DisconnectContactors.c
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osThreadSetPriority_ExpectAndReturn(commonContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, COMMON_OPENED, 0);
    
    orionInterface(&message);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.orionCanReceivedRecently, auxStatus.orionCanReceivedRecently, "auxStatus.orionCanReceivedRecently failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowCharge, auxStatus.allowCharge, "auxStatus.allowCharge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowDischarge, auxStatus.allowDischarge, "auxStatus.allowDischarge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.strobeBmsLight, auxStatus.strobeBmsLight, "auxStatus.strobeBmsLight failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxBmsContactorState.contactorsDisconnected, auxBmsContactorState.contactorsDisconnected, "auxBmsContactorState.contactorsDisconnected failed to update");
}

void test_shouldNotDisconnectContactorsChargeStateOpen() {
    AuxStatus expectedAuxStatus = {
        .orionCanReceivedRecently = 0,
        .allowCharge = 1,
        .allowDischarge = 1,
        .strobeBmsLight = 0
    };
    auxBmsContactorState.startupDone = 1;
    auxBmsContactorState.chargeState = OPEN;
    uint32_t contactorControlEventFlags = 0x4; // CHARGE_CLOSED

    osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, ORION_QUEUE_TIMEOUT, osErrorTimeout);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, 1);
    osMutexAcquire_ExpectAndReturn(auxTripMutex, MUTEX_TIMEOUT, osError);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, contactorControlEventFlags, 0);
    
    orionInterface(&message);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.orionCanReceivedRecently, auxStatus.orionCanReceivedRecently, "auxStatus.orionCanReceivedRecently failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowCharge, auxStatus.allowCharge, "auxStatus.allowCharge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowDischarge, auxStatus.allowDischarge, "auxStatus.allowDischarge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.strobeBmsLight, auxStatus.strobeBmsLight, "auxStatus.strobeBmsLight failed to update");
}

void test_shouldNotDisconnectContactorsChargeStateClosed() {
    AuxStatus expectedAuxStatus = {
        .orionCanReceivedRecently = 0,
        .allowCharge = 1,
        .allowDischarge = 1,
        .strobeBmsLight = 0
    };
    auxBmsContactorState.startupDone = 1;
    auxBmsContactorState.dischargeState == OPEN;
    auxBmsContactorState.chargeState = CLOSED;
    uint32_t contactorControlEventFlags = 0x10; // DISCHARGE_CLOSED

    osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, ORION_QUEUE_TIMEOUT, osErrorTimeout);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, 1);
    osMutexAcquire_ExpectAndReturn(auxTripMutex, MUTEX_TIMEOUT, osError);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, contactorControlEventFlags, 0);
    
    orionInterface(&message);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.orionCanReceivedRecently, auxStatus.orionCanReceivedRecently, "auxStatus.orionCanReceivedRecently failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowCharge, auxStatus.allowCharge, "auxStatus.allowCharge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.allowDischarge, auxStatus.allowDischarge, "auxStatus.allowDischarge failed to update");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxStatus.strobeBmsLight, auxStatus.strobeBmsLight, "auxStatus.strobeBmsLight failed to update");
}
