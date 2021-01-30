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



void setOrionInterfaceOsExpects(uint8_t orionCanReceivedRecently)
{
    if (orionCanReceivedRecently)
    {
        osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, 400, osOK);
    }
    else
    {
        osMessageQueueGet_ExpectAndReturn(orionInterfaceQueue, &message, NULL, 400, osErrorTimeout);
    }

    osMutexAcquire_ExpectAndReturn(auxTripMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxTripMutex, osOK);
    osMutexAcquire_ExpectAndReturn(auxStatusOrionInterfaceMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusOrionInterfaceMutex, osOK);
}

void setTripExpects(uint8_t chargeTrip, uint8_t dischargeTrip, uint8_t protectionTrip)
{
    checkChargeTrip_ExpectAndReturn(&message, chargeTrip);
    checkDischargeTrip_ExpectAndReturn(&message, dischargeTrip);
    checkProtectionTrip_ExpectAndReturn(&message, protectionTrip);
}

void setOrionEnableSenseExpects(GPIO_PinState expectedOrionDischargeSense, GPIO_PinState expectedOrionChargeSense)
{
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin, expectedOrionDischargeSense);
    HAL_GPIO_ReadPin_ExpectAndReturn(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin, expectedOrionChargeSense);
}

void setContactorState(ContactorState charge, ContactorState discharge, uint8_t startupDone)
{
    auxBmsContactorState.chargeState = charge;
    auxBmsContactorState.dischargeState = discharge;
    auxBmsContactorState.startupDone = startupDone;
}

void setContactorEventFlagsExpects(uint32_t contactorEventFlags)
{
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, contactorEventFlags, 0);
}

// Standard, normal operation - Orion allows contactors to be on
// and isn't reporting abormal cell voltages
// Test that car will work normally under normal conditions
void test_StandardOperationShouldUpdateStatusesNormally()
{
    setOrionInterfaceOsExpects(1);
    setTripExpects(0, 0, 0);
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;

    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    message.highCellVoltage = 30000;
    message.lowCellVoltage = 30000;

    // Explictly define what we are expecting
    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.strobeBmsLight = 0;
    expectedAuxStatus.allowCharge = 1;
    expectedAuxStatus.allowDischarge = 1;
    expectedAuxStatus.orionCanReceivedRecently = 1;
    expectedAuxStatus.chargeShouldTrip = 0;
    expectedAuxStatus.dischargeShouldTrip = 0;

    updateAuxTrip_Expect(&message, &localAuxTrip);

    orionInterface(&message);
    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion message hasn't been received recently via CAN
// Test that aux status is set appropriately
void test_NoOrionCanMessageReceivedRecently()
{
    setOrionInterfaceOsExpects(0);
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;

    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    // Explictly define what we are expecting
    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.orionCanReceivedRecently = 0;

    orionInterface(&message);
    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion requests to disconnect discharge contactor
// Test that the discharge contactor off event is triggered & statuses are correct
void test_NoDischargeSenseShouldTurnOffPinAndUpdateStatuses()
{
    setOrionInterfaceOsExpects(1);
    setTripExpects(0, 0, 0);
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(DISCHARGE_OPENED);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_RESET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowDischarge = 0;

    updateAuxTrip_Expect(&message, &localAuxTrip);

    orionInterface(&message);

    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion requests to disconnect charge contactor
// Test that the charge contactor off event is triggered & statuses are correct
void test_NoChargeSenseShouldTurnOffPinAndUpdateStatuses()
{
    setOrionInterfaceOsExpects(1);
    setTripExpects(0, 0, 0);
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(CHARGE_OPENED);
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_RESET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowCharge = 0;

    updateAuxTrip_Expect(&message, &localAuxTrip);

    orionInterface(&message);

    assertAuxStatusEqual(expectedAuxStatus);
}

// It's determined that the car should trip
// Test that this task will call disconnect contactors & update statuses
void test_ShouldTripWillDisconnectContactorsAndUpdateStatuses()
{
    setOrionInterfaceOsExpects(1);
    setTripExpects(1, 1, 1); // Will just choose protection. Any of them should work
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(COMMON_OPENED);
    osThreadSetPriority_ExpectAndReturn(commonContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowCharge = 0;
    expectedAuxStatus.allowDischarge = 0;
    expectedAuxStatus.strobeBmsLight = 1;
    expectedAuxStatus.dischargeShouldTrip = 1;
    expectedAuxStatus.chargeShouldTrip = 1;

    updateAuxTrip_Expect(&message, &localAuxTrip);

    orionInterface(&message);

    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion reports a cell voltage above the internal Aux BMS range
// Test that the charge contactor off event is triggered & statuses are correct
void test_TooHighHighCellVoltageShouldTurnOffChargeAndUpdateStatuses()
{
    setOrionInterfaceOsExpects(1);
    setTripExpects(0, 0, 0);
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(CHARGE_OPENED);
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);

    message.lowCellVoltage = 30000;
    message.highCellVoltage = 41600;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowCharge = 0;

    updateAuxTrip_Expect(&message, &localAuxTrip);

    orionInterface(&message);

    assertAuxStatusEqual(expectedAuxStatus);
}

// Orion reports a cell voltage is below the internal Aux BMS range
// Test that the discharge contactor will be closed & statuses are updated
void test_TooLowLowCellVoltageShouldTurnOffDischargeAndUpdateStatuses()
{
    setOrionInterfaceOsExpects(1);
    setTripExpects(0, 0, 0);
    setContactorState(CLOSED, CLOSED, 1);
    setContactorEventFlagsExpects(DISCHARGE_OPENED);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);

    GPIO_PinState expectedOrionDischargeSense = GPIO_PIN_SET;
    GPIO_PinState expectedOrionChargeSense = GPIO_PIN_SET;
    setOrionEnableSenseExpects(expectedOrionDischargeSense, expectedOrionChargeSense);


    message.lowCellVoltage = 25900;
    message.highCellVoltage = 30000;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.allowDischarge = 0;

    updateAuxTrip_Expect(&message, &localAuxTrip);

    orionInterface(&message);

    assertAuxStatusEqual(expectedAuxStatus);
}

void runOrionInterfaceTests()
{
    RUN_TEST(test_StandardOperationShouldUpdateStatusesNormally);
    RUN_TEST(test_NoOrionCanMessageReceivedRecently);
    RUN_TEST(test_NoDischargeSenseShouldTurnOffPinAndUpdateStatuses);
    RUN_TEST(test_NoChargeSenseShouldTurnOffPinAndUpdateStatuses);
    RUN_TEST(test_ShouldTripWillDisconnectContactorsAndUpdateStatuses);
    RUN_TEST(test_TooHighHighCellVoltageShouldTurnOffChargeAndUpdateStatuses);
    RUN_TEST(test_TooLowLowCellVoltageShouldTurnOffDischargeAndUpdateStatuses);
}
