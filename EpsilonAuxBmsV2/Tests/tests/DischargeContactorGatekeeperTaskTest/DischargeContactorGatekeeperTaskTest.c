#include "DischargeContactorGatekeeperTaskTest.h"


osEventFlagsId_t contactorControlEventBits;
AuxBmsContactorState auxBmsContactorState;
osThreadId_t dischargeContactorGatekeeperTaskHandle;

void dischargeOpenedTest()
{
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever, DISCHARGE_OPENED);
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityNormal, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, OPEN, "Discharge contactor state not OPEN");
}

void dischargeClosedWhileChargeOpenedSuccessfullyTest()
{
    auxBmsContactorState.chargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever, DISCHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(2, 1);
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CLOSED, "Discharge contactor state not CLOSED");
}

void dischargeClosedWhileChargeClosedSuccessfullyTest()
{
    auxBmsContactorState.chargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever, DISCHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 1);
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CLOSED, "Discharge contactor state not CLOSED");
}

void dischargeClosedUnsuccessfullyWhileChargeOpenedDueToSenseTest()
{
    auxBmsContactorState.chargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever, DISCHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_SET);
    isCurrentLow_ExpectAndReturn(2, 1);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, CHARGE_CLOSED, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CONTACTOR_ERROR, "Discharge contactor state not CONTACTOR_ERROR");
}

void dischargeClosedUnsuccessfullyWhileChargeClosedDueToCurrentTest()
{
    auxBmsContactorState.chargeState = CLOSED;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever, DISCHARGE_CLOSED);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, GPIO_PIN_RESET);
    isCurrentLow_ExpectAndReturn(3, 0);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    osDelay_ExpectAndReturn(CONTACTOR_DELAY, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED, 0);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, CONTACTOR_ERROR, "Discharge contactor state not CONTACTOR_ERROR");
}

void dischargeClosedButContactorsDisconnectedTest()
{
    auxBmsContactorState.contactorsDisconnected = 1;
    auxBmsContactorState.dischargeState = OPEN;
    osEventFlagsWait_ExpectAndReturn(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever, DISCHARGE_CLOSED);
    dischargeContactorGatekeeper();
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.dischargeState, OPEN, "Discharge contactor state not OPEN");
}



int runDischargeContactorGatekeeperTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(dischargeOpenedTest);
    RUN_TEST(dischargeClosedWhileChargeOpenedSuccessfullyTest);
    RUN_TEST(dischargeClosedWhileChargeClosedSuccessfullyTest);
    RUN_TEST(dischargeClosedUnsuccessfullyWhileChargeOpenedDueToSenseTest);
    RUN_TEST(dischargeClosedUnsuccessfullyWhileChargeClosedDueToCurrentTest);
    RUN_TEST(dischargeClosedButContactorsDisconnectedTest);

    return UNITY_END();
}
