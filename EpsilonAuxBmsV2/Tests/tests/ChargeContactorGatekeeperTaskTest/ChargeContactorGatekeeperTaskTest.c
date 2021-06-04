#include "ChargeContactorGatekeeperTaskTest.h"


osEventFlagsId_t contactorControlEventBits;
AuxBmsContactorState auxBmsContactorState;
osThreadId_t chargeContactorGatekeeperTaskHandle;

void chargeOpenedTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED | CHARGE_OPENED, osFlagsWaitAny, osWaitForever, CHARGE_OPENED);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityNormal, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, OPEN, "Charge contactor state not OPEN");
}

void chargeClosedWhileDischargeOpenedSuccessfullyTest()
{
    auxBmsContactorState.dischargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED | CHARGE_OPENED, osFlagsWaitAny, osWaitForever, CHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(2, 1);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CLOSED, "Charge contactor state not CLOSED");
}

void chargeClosedWhileDischargeClosedSuccessfullyTest()
{
    auxBmsContactorState.dischargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED | CHARGE_OPENED, osFlagsWaitAny, osWaitForever, CHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 1);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CLOSED, "Charge contactor state not CLOSED");
}

void chargeClosedUnsuccessfullyWhileDischargeOpenedDueToSenseTest()
{
    auxBmsContactorState.dischargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED | CHARGE_OPENED, osFlagsWaitAny, osWaitForever, CHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_SET);
    isCurrentLow_ExpectAndReturn(2, 1);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CONTACTOR_ERROR, "Charge contactor state not CONTACTOR_ERROR");
}

void chargeClosedUnsuccessfullyWhileDischargeClosedDueToCurrentTest()
{
    auxBmsContactorState.dischargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED | CHARGE_OPENED, osFlagsWaitAny, osWaitForever, CHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 0);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CONTACTOR_ERROR, "Charge contactor state not CONTACTOR_ERROR");
}

void chargeClosedButContactorsDisconnectedTest()
{
    auxBmsContactorState.contactorsDisconnected = 1;
    auxBmsContactorState.chargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED | CHARGE_OPENED, osFlagsWaitAny, osWaitForever, CHARGE_CLOSED);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, OPEN, "Charge contactor state not OPEN");
}



int runChargeContactorGatekeeperTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(chargeOpenedTest);
    RUN_TEST(chargeClosedWhileDischargeOpenedSuccessfullyTest);
    RUN_TEST(chargeClosedWhileDischargeClosedSuccessfullyTest);
    RUN_TEST(chargeClosedUnsuccessfullyWhileDischargeOpenedDueToSenseTest);
    RUN_TEST(chargeClosedUnsuccessfullyWhileDischargeClosedDueToCurrentTest);
    RUN_TEST(chargeClosedButContactorsDisconnectedTest);

    return UNITY_END();
}
