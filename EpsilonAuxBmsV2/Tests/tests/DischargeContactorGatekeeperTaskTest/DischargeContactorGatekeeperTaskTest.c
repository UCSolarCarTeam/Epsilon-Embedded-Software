#include <string.h>
#include "unity.h"

#include "DischargeContactorGatekeeperTaskTest.h"
#include "Mockcmsis_os2.h"
#include "ContactorState.h"
#include "MockCheckCurrent.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "ContactorEventFlags.h"
#include "DischargeContactorGatekeeperTask.h"

osEventFlagsId_t contactorControlEventBits;
AuxBmsContactorState auxBmsContactorState;
osThreadId_t dischargeContactorGatekeeperTaskHandle;

void dischargeOffTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON | DISCHARGE_OFF, osFlagsWaitAny, osWaitForever, DISCHARGE_OFF);
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityNormal, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, OPEN, "Discharge contactor state not OPEN");
}

void dischargeOnWhileChargeOffSuccessfullyTest()
{
    auxBmsContactorState.chargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON | DISCHARGE_OFF, osFlagsWaitAny, osWaitForever, DISCHARGE_ON);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(2, 1);
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_ON, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CLOSED, "Discharge contactor state not CLOSED");
}

void dischargeOnWhileChargeOnSuccessfullyTest()
{
    auxBmsContactorState.chargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON | DISCHARGE_OFF, osFlagsWaitAny, osWaitForever, DISCHARGE_ON);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 1);
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CLOSED, "Discharge contactor state not CLOSED");
}

void dischargeOnUnsuccessfullyWhileChargeOffDueToSenseTest()
{
    auxBmsContactorState.chargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON | DISCHARGE_OFF, osFlagsWaitAny, osWaitForever, DISCHARGE_ON);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_SET);
    isCurrentLow_ExpectAndReturn(2, 1);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_ON, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CONTACTOR_ERROR, "Discharge contactor state not CONTACTOR_ERROR");
}

void dischargeOnUnsuccessfullyWhileChargeOnDueToCurrentTest()
{
    auxBmsContactorState.chargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON | DISCHARGE_OFF, osFlagsWaitAny, osWaitForever, DISCHARGE_ON);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 0);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CONTACTOR_ERROR, "Discharge contactor state not CONTACTOR_ERROR");
}

void dischargeOnButContactorsDisconnectedTest()
{
    auxBmsContactorState.contactorsDisconnected = 1;
    auxBmsContactorState.dischargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_ON | DISCHARGE_OFF, osFlagsWaitAny, osWaitForever, DISCHARGE_ON);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, OPEN, "Discharge contactor state not OPEN");
}



int runDischargeContactorGatekeeperTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(dischargeOffTest);
    RUN_TEST(dischargeOnWhileChargeOffSuccessfullyTest);
    RUN_TEST(dischargeOnWhileChargeOnSuccessfullyTest);
    RUN_TEST(dischargeOnUnsuccessfullyWhileChargeOffDueToSenseTest);
    RUN_TEST(dischargeOnUnsuccessfullyWhileChargeOnDueToCurrentTest);
    RUN_TEST(dischargeOnButContactorsDisconnectedTest);

    return UNITY_END();
}
