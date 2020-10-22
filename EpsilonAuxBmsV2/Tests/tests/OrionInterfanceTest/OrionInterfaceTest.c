#include "OrionInterfaceTask.h"
#include "CanRxInterruptParserTask.h"
#include "AuxStatusHelper.h"

void runOrionInterfaceTests()
{
    RUN_TEST(test_StandardOperationShouldUpdateStatusesNormally);
    RUN_TEST(test_NoDischargeSenseShouldTurnOffPinAndUpdateStatuses);
    RUN_TEST(test_NoChargeSenseShouldTurnOffPinAndUpdateStatuses);
    RUN_TEST(test_ShouldTripWillDisconnectContactorsAndUpdateStatuses);
    RUN_TEST(test_TooHighMaxCellVoltageShouldTurnOffChargeAndUpdateStatuses);
    RUN_TEST(test_TooLowMinCellVoltageShouldTurnOffDischargeAndUpdateStatuses);
}

void setUpdateChargeAllowanceOsExpects(uint32_t* prevWakeTime)
{
    osDelayUntil_ExpectAndReturn(prevWakeTime, CHARGE_ALLOWANCE_UPDATE_FREQ, osOK);
    osMutexWait_ExpectAndReturn(auxStatus.auxStatusMutex, 0, osOK);
    osMutexRelease_ExpectAndReturn(auxStatus.auxStatusMutex, osOK);
    osMutexWait_ExpectAndReturn(orionStatus.orionStatusMutex, 0, osOK);
    osMutexRelease_ExpectAndReturn(orionStatus.orionStatusMutex, osOK);
}

void setOrionEnableSenseExpects(GPIO_PinState expectedOrionDischargeSense, GPIO_PinState expectedOrionChargeSense)
{
    auxStatus.startUpSequenceDone = 1;
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, expectedOrionDischargeSense);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, expectedOrionChargeSense);
}

void test_StandardOperationShouldUpdateStatusesNormally()
{
    
}