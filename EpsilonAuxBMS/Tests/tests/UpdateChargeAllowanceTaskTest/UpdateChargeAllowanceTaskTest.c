#include "UpdateChargeAllowanceTaskTest.h"
#include "unity.h"

#include "Mockcmsis_os.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "MockStateFunctionsShared.h"
#include "UpdateChargeAllowanceTask.h"
#include "MockTrip.h"
#include "AuxStatusHelper.h"
#include "OrionStatusHelper.h"
#include "stdio.h"

int runUpdateChargeAllowanceTaskTests()
{
    UNITY_BEGIN();

    RUN_TEST(test_StandardOperationShouldUpdateStatusesNormally);
    RUN_TEST(test_NoDischargeSenseShouldTurnOffPinAndUpdateStatuses);
    RUN_TEST(test_NoChargeSenseShouldTurnOffPinAndUpdateStatuses);
    RUN_TEST(test_ShouldTripWillDisconnectContactorsAndUpdateStatuses);
    RUN_TEST(test_TooHighMaxCellVoltageShouldTurnOffChargeAndUpdateStatuses);
    RUN_TEST(test_TooLowMinCellVoltageShouldTurnOffDischargeAndUpdateStatuses);

    return UNITY_END();
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

// Standard, normal operation - Orion allows contactors to be on
// and isn't reporting abormal cell voltages
// Test that car will work normally under normal conditions
void test_StandardOperationShouldUpdateStatusesNormally()
{
    uint32_t prevWakeTime = 0;
    setUpdateChargeAllowanceOsExpects(&prevWakeTime);
    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;

    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    carShouldTrip_ExpectAndReturn(0);

    orionStatus.canMsgReceived = 1;
    orionStatus.maxCellVoltage = 30000;
    orionStatus.minCellVoltage = 30000;

    // Explictly define what we are expecting
    OrionStatus expectedOrionStatus = orionStatus;
    expectedOrionStatus.allowDischarge = 1;
    expectedOrionStatus.allowCharge = 1;
    expectedOrionStatus.batteryVoltagesInRange = 1;
    expectedOrionStatus.shutOff = 0;
    expectedOrionStatus.chargeContactorOverriden = 0;
    expectedOrionStatus.dischargeContactorOverriden = 0;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.strobeBmsLight = 0;
    expectedAuxStatus.allowCharge = 1;

    updateChargeAllowance(&prevWakeTime);

    assertOrionStatusEqual(expectedOrionStatus);
    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion requests to disconnect discharge contactor
// Test that discharge and HV are reset & statuses are correct
void test_NoDischargeSenseShouldTurnOffPinAndUpdateStatuses()
{
    uint32_t prevWakeTime = 0;
    setUpdateChargeAllowanceOsExpects(&prevWakeTime);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_RESET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    carShouldTrip_ExpectAndReturn(0);

    // Test if PIN discharge enable and HV was written to
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);

    OrionStatus expectedOrionStatus = orionStatus;
    expectedOrionStatus.allowDischarge = 0;
    expectedOrionStatus.dischargeContactorOverriden = 1;

    AuxStatus expectedAuxStatus = auxStatus;

    updateChargeAllowance(&prevWakeTime);

    assertOrionStatusEqual(expectedOrionStatus);
    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion requests to disconnect charge contactor
// Test that charge is reset & statuses are correct
void test_NoChargeSenseShouldTurnOffPinAndUpdateStatuses()
{
    uint32_t prevWakeTime = 0;
    setUpdateChargeAllowanceOsExpects(&prevWakeTime);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_RESET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    carShouldTrip_ExpectAndReturn(0);

    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);

    OrionStatus expectedOrionStatus = orionStatus;
    expectedOrionStatus.allowCharge = 0;
    expectedOrionStatus.chargeContactorOverriden = 1;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowCharge = 0;

    updateChargeAllowance(&prevWakeTime);

    assertOrionStatusEqual(expectedOrionStatus);
    assertAuxStatusEqual(expectedAuxStatus);
}

// It's determined that the car should trip
// Test that this task will call disconnect contactors & update statuses
void test_ShouldTripWillDisconnectContactorsAndUpdateStatuses()
{
    uint32_t prevWakeTime = 0;
    setUpdateChargeAllowanceOsExpects(&prevWakeTime);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    carShouldTrip_ExpectAndReturn(1);

    disconnectContactors_Expect(0);
    orionStatus.canMsgReceived = 1;

    OrionStatus expectedOrionStatus = orionStatus;
    expectedOrionStatus.allowCharge = 0;
    expectedOrionStatus.chargeContactorOverriden = 1;
    expectedOrionStatus.allowDischarge = 0;
    expectedOrionStatus.dischargeContactorOverriden = 1;
    expectedOrionStatus.shutOff = 1;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowCharge = 0;
    expectedAuxStatus.strobeBmsLight = 1;

    updateChargeAllowance(&prevWakeTime);

    assertOrionStatusEqual(expectedOrionStatus);
    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion reports a cell voltage above the internal Aux BMS range
// Test that the charge contactor will be closed & statuses are updated
void test_TooHighMaxCellVoltageShouldTurnOffChargeAndUpdateStatuses()
{
    uint32_t prevWakeTime = 0;
    setUpdateChargeAllowanceOsExpects(&prevWakeTime);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    carShouldTrip_ExpectAndReturn(0);

    orionStatus.canMsgReceived = 1;
    orionStatus.maxCellVoltage = 41600;

    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);

    OrionStatus expectedOrionStatus = orionStatus;
    expectedOrionStatus.batteryVoltagesInRange = 0;
    expectedOrionStatus.allowCharge = 0;
    expectedOrionStatus.chargeContactorOverriden = 1;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowCharge = 0;

    updateChargeAllowance(&prevWakeTime);

    assertOrionStatusEqual(expectedOrionStatus);
    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion reports a cell voltage is below the internal Aux BMS range
// Test that the discharge contactor will be closed & statuses are updated
void test_TooLowMinCellVoltageShouldTurnOffDischargeAndUpdateStatuses()
{
    uint32_t prevWakeTime = 0;
    setUpdateChargeAllowanceOsExpects(&prevWakeTime);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    carShouldTrip_ExpectAndReturn(0);

    orionStatus.canMsgReceived = 1;
    orionStatus.minCellVoltage = 25900;

    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);

    OrionStatus expectedOrionStatus = orionStatus;
    expectedOrionStatus.batteryVoltagesInRange = 0;
    expectedOrionStatus.allowDischarge = 0;
    expectedOrionStatus.dischargeContactorOverriden = 1;

    AuxStatus expectedAuxStatus = auxStatus;

    updateChargeAllowance(&prevWakeTime);

    assertOrionStatusEqual(expectedOrionStatus);
    assertAuxStatusEqual(expectedAuxStatus);
}