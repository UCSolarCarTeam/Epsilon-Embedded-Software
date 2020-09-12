#include <string.h>
#include "unity.h"

#include "ChargeContactorGatekeeperTaskTest.h"
#include "Mockcmsis_os2.h"
#include "ContactorState.h"
#include "MockCheckCurrent.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "ContactorEventFlags.h"
#include "ChargeContactorGatekeeperTask.h"

osEventFlagsId_t contactorControlEventBits;
AuxBmsContactorState auxBmsContactorState;
osThreadId_t chargeContactorGatekeeperTaskHandle;

void chargeOffTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever, CHARGE_OFF);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityNormal, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, OPEN, "Charge contactor state not OPEN");
}

void chargeOnWhileDischargeOffSuccessfullyTest()
{
    auxBmsContactorState.dischargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever, CHARGE_ON);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(2, 1);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CLOSED, "Charge contactor state not CLOSED");
}

void chargeOnWhileDischargeOnSuccessfullyTest()
{
    auxBmsContactorState.dischargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever, CHARGE_ON);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 1);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CLOSED, "Charge contactor state not CLOSED");
}

void chargeOnUnsuccessfullyWhileDischargeOffDueToSenseTest()
{
    auxBmsContactorState.dischargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever, CHARGE_ON);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_SET);
    isCurrentLow_ExpectAndReturn(2, 1);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CONTACTOR_ERROR, "Charge contactor state not CONTACTOR_ERROR");
}

void chargeOnUnsuccessfullyWhileDischargeOnDueToCurrentTest()
{
    auxBmsContactorState.dischargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever, CHARGE_ON);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 0);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_ON, 0);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, CONTACTOR_ERROR, "Charge contactor state not CONTACTOR_ERROR");
}

void chargeOnButContactorsDisconnectedTest()
{
    auxBmsContactorState.contactorsDisconnected = 1;
    auxBmsContactorState.chargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever, CHARGE_ON);
    chargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.chargeState, OPEN, "Charge contactor state not OPEN");
}



int runChargeContactorGatekeeperTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(chargeOffTest);
    RUN_TEST(chargeOnWhileDischargeOffSuccessfullyTest);
    RUN_TEST(chargeOnWhileDischargeOnSuccessfullyTest);
    RUN_TEST(chargeOnUnsuccessfullyWhileDischargeOffDueToSenseTest);
    RUN_TEST(chargeOnUnsuccessfullyWhileDischargeOnDueToCurrentTest);
    RUN_TEST(chargeOnButContactorsDisconnectedTest);

    return UNITY_END();
}
