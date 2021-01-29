#include <string.h>
#include "unity.h"

#include "CommonContactorGatekeeperTaskTest.h"
#include "Mockcmsis_os2.h"
#include "ContactorState.h"
#include "MockCheckCurrent.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "ContactorEventFlags.h"
#include "CommonContactorGatekeeperTask.h"

osEventFlagsId_t contactorControlEventBits;
AuxBmsContactorState auxBmsContactorState;
osThreadId_t commonContactorGatekeeperTaskHandle;

void commonOpenedTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED | COMMON_OPENED, osFlagsWaitAny, osWaitForever, COMMON_OPENED);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_OPENED | CHARGE_OPENED, 0);
    osThreadSetPriority_ExpectAndReturn(commonContactorGatekeeperTaskHandle, osPriorityNormal, 0);
    commonContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, OPEN, "Common contactor state not OPEN");
}

void commonClosedSuccessfullyTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED | COMMON_OPENED, osFlagsWaitAny, osWaitForever, COMMON_CLOSED);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(1, 1);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED, 0);
    commonContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, CLOSED, "Common contactor state not CLOSED");
}

void commonCannotCloseDueToSenseTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED | COMMON_OPENED, osFlagsWaitAny, osWaitForever, COMMON_CLOSED);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin, GPIO_PIN_SET);
    isCurrentLow_ExpectAndReturn(1, 1);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED, 0);
    commonContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, CONTACTOR_ERROR, "Common contactor state not CONTACTOR_ERROR");
}

void commonClosedUnsuccessfullyDueToCurrentTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED | COMMON_OPENED, osFlagsWaitAny, osWaitForever, COMMON_CLOSED);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(1, 0);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED, 0);
    commonContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, CONTACTOR_ERROR, "Common contactor state not CONTACTOR_ERROR");
}

void tryToCloseCommonWhileContactorsDisconnected()
{
    auxBmsContactorState.contactorsDisconnected = 1;
    auxBmsContactorState.commonState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, COMMON_CLOSED | COMMON_OPENED, osFlagsWaitAny, osWaitForever, COMMON_CLOSED);
    commonContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, OPEN, "Common contactor state not OPEN");
}



int runCommonContactorGatekeeperTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(commonOpenedTest);
    RUN_TEST(commonClosedSuccessfullyTest);
    RUN_TEST(commonCannotCloseDueToSenseTest);
    RUN_TEST(commonClosedUnsuccessfullyDueToCurrentTest);
    RUN_TEST(tryToCloseCommonWhileContactorsDisconnected);

    return UNITY_END();
}
