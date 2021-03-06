#include "unity.h"

#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockstm32f4xx_hal_can.h"
//#include "Mockstm32f4xx_hal_adc.h"
#include "Mockcmsis_os.h"

//#include "stm32f4xx_hal_can.h"
#include "Lights.h"
#include "LightsTest.h"
#include <stdio.h>

updateLights lightCharacteristics = {0};

uint8_t lightsInputs;
uint8_t auxBmsInputs[2];
uint8_t batteryErrors[5];
uint8_t driversInputs[4];
SigLightsHandle sigLightsHandle;

osMutexId* canHandleMutex;

void runLightsTests()
{
    RUN_TEST(test_headLightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff);
    RUN_TEST(test_headLightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn);
    RUN_TEST(test_headLightsHigh_headLights_Low_breakLightsOn_hazardsOn_bmsStrobeLightOn);
    RUN_TEST(blinkSignalLights_sigLightsLeftOFF_sigLightsRightOFF);
    RUN_TEST(blinkSignalLights_sigLightsLeftON_sigLightsRightON_prevSigState_Zero);
    RUN_TEST(blinkSignalLights_prevSigState0_sigLightsLeftON_sigLightsRightON);
    RUN_TEST(updateStrobeLights2_ESTROBEON_UpdateblinkerTimer);
    RUN_TEST(updateStrobeLights2_ESTROBEOFF_ResetBlinkerTimer);
    RUN_TEST(updateStrobeLights2_ESTROBEOFF_UpdateBlinkerTimer);
    RUN_TEST(reportLightsToCan_test);
    RUN_TEST(sendHeartbeat_test);
    RUN_TEST(HAL_CAN_RxCpltCallback_Lights_Input_StdId_test);
    RUN_TEST(HAL_CAN_RxCpltCallback_Battery_Stat_Errors_StdId_test);
    RUN_TEST(HAL_CAN_RxCpltCallback_Drivers_Inputs_StdId_test);
    RUN_TEST(HAL_CAN_RxCpltCallback_AuxBms_Input_StdId_test);
    RUN_TEST(HAL_CAN_RxCpltCallback_Battery_Stat_Errors_StdId_AND_Toggle_Red_Pin_test);
}

void test_headLightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff()
{
    lightsInputs = 0b00000001;
    driversInputs[1] = 0;
    driversInputs[2] = 0;
    auxBmsInputs[1] = 0;
    driversInputs[3] = 0b00000000;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 1;
    lightCharacteristics.rightSignal = 0;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    //when headlights are on
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);

    //brake lights are off
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);

    //bmsstrobe off
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);

    updateLights1(&lightCharacteristics, &prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(lightCharacteristics.leftSignal, sigLightsHandle.left,
                              "sigLightsHandle.left expected to be 1");
    TEST_ASSERT_EQUAL_MESSAGE(lightCharacteristics.rightSignal, sigLightsHandle.right,
                              "sigLightsHandle.right expected to be 0");

}

void test_headLightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn()
{
    lightsInputs = 0b00100010;
    driversInputs[3] = 0b00000001;
    driversInputs[1] = 0b00010000;
    driversInputs[2] = 0b00000001;
    auxBmsInputs[1] = 0b00000001;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 0;
    lightCharacteristics.rightSignal = 0;

    uint8_t headLightsHigh = 0;
    uint8_t headLightsLow = 1;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    //Headlights Low
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, headLightsHigh);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, headLightsLow);
    // Brake lights on
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_ON);
    //bmsStrobe on
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);

    updateLights1(&lightCharacteristics, &prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.left,
                              "sigLightsHandle.left expected to be LIGHT_ON");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.right,
                              "sigLightsHandle.right expected to be LIGHT_ON");


}

void test_headLightsHigh_headLights_Low_breakLightsOn_hazardsOn_bmsStrobeLightOn()
{
    lightsInputs = 0b0100110;
    driversInputs[3] = 0b00000001;
    driversInputs[1] = 0b00010000;
    driversInputs[2] = 0b00000001;
    auxBmsInputs[1] = 0b00000001;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 0;
    lightCharacteristics.rightSignal = 0;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    //Headlights Low and High
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, LIGHT_ON);
    // Brake lights on
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_ON);
    //bmsStrobe on
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);

    updateLights1(&lightCharacteristics, &prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.left,
                              "sigLightsHandle.left expected to be LIGHT_ON");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.right,
                              "sigLightsHandle.right expected to be LIGHT_ON");

}

//TESTS for blinkSignalLIghts

void blinkSignalLights_sigLightsLeftOFF_sigLightsRightOFF()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 10;
    uint32_t blinkerTimer = 0;

    sigLightsHandle.left = 0;
    sigLightsHandle.right = 0;


    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);

    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);

    TEST_ASSERT_EQUAL_MESSAGE(0, prevSigState, "prevSigState is supposed to be 0");


}

void blinkSignalLights_sigLightsLeftON_sigLightsRightON_prevSigState_Zero()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 0;
    uint32_t blinkerTimer = 0;


    sigLightsHandle.left = 1;
    sigLightsHandle.right = 1;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, sigLightsHandle.right);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, sigLightsHandle.left);

    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);
    TEST_ASSERT_EQUAL_MESSAGE(0, blinkerTimer, "blinkerTimer is supposed to be 0");
    TEST_ASSERT_EQUAL_MESSAGE(1, prevSigState, "prevSigState is supposed to be 1");

}

void blinkSignalLights_prevSigState0_sigLightsLeftON_sigLightsRightON()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 1;


    sigLightsHandle.left = 2;
    sigLightsHandle.right = 2;

    keepBlinkersOFF_resetBlinkerTimerToZero(prevWakeTime, prevSigState);
    turnBlinkersON_updateTimer(prevWakeTime, prevSigState);
    keepBlinkersOFF_updateTimer(prevWakeTime, prevSigState);

}

void keepBlinkersOFF_resetBlinkerTimerToZero(uint32_t prevWakeTime, uint8_t prevSigState)
{
    uint32_t blinkerTimer = 771;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);
    TEST_ASSERT_EQUAL_MESSAGE(0, blinkerTimer, "blinkerTimer is supposed to be 0");

}

void turnBlinkersON_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState)
{
    uint32_t blinkerTimer = 300;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, sigLightsHandle.right);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, sigLightsHandle.left);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);
    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer, "blinkerTimer is supposed to be expectedBlinkerTimer");

}

void keepBlinkersOFF_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState)
{
    uint32_t blinkerTimer = 400;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);
    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer, "blinkerTimer is supposed to be expectedBlinkerTimer");

}


// TESTS for updateStrobeLight2

void updateStrobeLights2_ESTROBEON_UpdateblinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 300;
    char strobeLight;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;

    auxBmsInputs[1] = 0b00000001;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
    updateStrobeLight2(&prevWakeTime, &blinkerTimer, &strobeLight);
    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer, "blinkerTimer is supposed to be expectedBlinkerTimer");

}

void updateStrobeLights2_ESTROBEOFF_ResetBlinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 800;
    char strobeLight;
    uint32_t expectedBlinkerTimer = 0;
    auxBmsInputs[1] = 0b0;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
    updateStrobeLight2(&prevWakeTime, &blinkerTimer, &strobeLight);
    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer,
                              "blinkerTimer is supposed to be expectedBlinkerTimer");

}

void updateStrobeLights2_ESTROBEOFF_UpdateBlinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 400;
    char strobeLight;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;
    auxBmsInputs[1] = 0b1;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
    updateStrobeLight2(&prevWakeTime, &blinkerTimer, &strobeLight);
    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer,
                              "blinkerTimer is supposed to be expectedBlinkerTimer");

}

// TESTS for reportLightsToCan
void reportLightsToCan_test()
{
    uint32_t prevWakeTime = 0;
    uint32_t expectedValue = 23;



    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_STATUS_FREQ, 0);

    osMutexWait_ExpectAndReturn(canHandleMutex, 0, osOK);
    HAL_GPIO_TogglePin_Expect(LED_BLUE_GPIO_Port, LED_BLUE_Pin);


    HAL_GPIO_ReadPin_ExpectAndReturn(HLOW_GPIO_Port, HLOW_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(HHIGH_GPIO_Port, HHIGH_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(BRAKE_GPIO_Port, BRAKE_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(LSIGNAL_GPIO_Port, LSIGNAL_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(RSIGNAL_GPIO_Port, RSIGNAL_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(ESTROBE_GPIO_Port, ESTROBE_Pin, 0);

    HAL_CAN_Transmit_IT_ExpectAndReturn(&hcan2, 0);
    osMutexRelease_ExpectAndReturn(canHandleMutex, 0);

    reportLightsToCan(&prevWakeTime, canHandleMutex);

    TEST_ASSERT_EQUAL_MESSAGE(expectedValue, hcan2.pTxMsg->Data[0], "expected value is supposed to be 57");

}

// TEST for sendHeartbeat
void sendHeartbeat_test()
{
    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_HEARTBEAT_FREQ, 0);
    osMutexWait_ExpectAndReturn(canHandleMutex, 0, osOK);
    HAL_GPIO_TogglePin_Expect(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    HAL_CAN_Transmit_IT_ExpectAndReturn(&hcan2, 0);
    osMutexRelease_ExpectAndReturn(canHandleMutex, 0);

    sendHeartbeat(canHandleMutex, &prevWakeTime);


    TEST_ASSERT_EQUAL_MESSAGE(LIGHTS_HEARTBEAT_STDID, hcan2.pTxMsg->StdId, "expected to be LIGHTS_HEARTBEAT_STDID value");
    TEST_ASSERT_EQUAL_MESSAGE(1, hcan2.pTxMsg->Data[0], "hcan2.pTxMsg->Data[0] should be 1");
}

//TESTS for HAL_CAN_RxCpltCallback
void HAL_CAN_RxCpltCallback_Lights_Input_StdId_test()
{
    hcan2.pRxMsg->StdId = LIGHTS_INPUT_STDID;
    hcan2.pRxMsg->DLC = 1;
    hcan2.pRxMsg->Data[0] = 2;

    HAL_CAN_Receive_IT_ExpectAndReturn(&hcan2, CAN_FIFO0, HAL_OK);

    HAL_CAN_RxCpltCallback(&hcan2);


    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[0], lightsInputs, "The actual value is supposed to be 2");

}

void HAL_CAN_RxCpltCallback_Battery_Stat_Errors_StdId_test()
{
    hcan2.pRxMsg->StdId = BATTERY_STAT_ERRORS_STDID;
    hcan2.pRxMsg->DLC = 5;
    hcan2.pRxMsg->Data[0] = 2;
    hcan2.pRxMsg->Data[1] = 1;
    hcan2.pRxMsg->Data[2] = 0;
    hcan2.pRxMsg->Data[3] = 1;
    hcan2.pRxMsg->Data[4] = 2;

    HAL_CAN_Receive_IT_ExpectAndReturn(&hcan2, CAN_FIFO0, HAL_OK);

    HAL_CAN_RxCpltCallback(&hcan2);


    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[0], batteryErrors[0], "The actual value is supposed to be 2");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[1], batteryErrors[1], "The actual value is supposed to be 1");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[2], batteryErrors[2], "The actual value is supposed to be 0");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[3], batteryErrors[3], "The actual value is supposed to be 1");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[4], batteryErrors[4], "The actual value is supposed to be 2");

}

void HAL_CAN_RxCpltCallback_Drivers_Inputs_StdId_test()
{
    hcan2.pRxMsg->StdId = DRIVERS_INPUTS_STDID;
    hcan2.pRxMsg->DLC = 4;
    hcan2.pRxMsg->Data[0] = 5;
    hcan2.pRxMsg->Data[1] = 4;
    hcan2.pRxMsg->Data[2] = 3;
    hcan2.pRxMsg->Data[3] = 2;

    HAL_CAN_Receive_IT_ExpectAndReturn(&hcan2, CAN_FIFO0, HAL_OK);

    HAL_CAN_RxCpltCallback(&hcan2);

    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[0], driversInputs[0], "The actual value is supposed to be 5");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[1], driversInputs[1], "The actual value is supposed to be 4");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[2], driversInputs[2], "The actual value is supposed to be 3");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[3], driversInputs[3], "The actual value is supposed to be 2");

}

void HAL_CAN_RxCpltCallback_AuxBms_Input_StdId_test()
{
    hcan2.pRxMsg->StdId = AUXBMS_INPUT_STDID;
    hcan2.pRxMsg->DLC = 2;
    hcan2.pRxMsg->Data[0] = 5;
    hcan2.pRxMsg->Data[1] = 6;

    HAL_CAN_Receive_IT_ExpectAndReturn(&hcan2, CAN_FIFO0, HAL_OK);

    HAL_CAN_RxCpltCallback(&hcan2);


    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[0], auxBmsInputs[0], "The actual value is supposed to be 5");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[1], auxBmsInputs[1], "The actual value is supposed to be 6");
}

void HAL_CAN_RxCpltCallback_Battery_Stat_Errors_StdId_AND_Toggle_Red_Pin_test()
{
    hcan2.pRxMsg->StdId = AUXBMS_INPUT_STDID;
    hcan2.pRxMsg->DLC = 2;
    hcan2.pRxMsg->Data[0] = 5;
    hcan2.pRxMsg->Data[1] = 6;

    HAL_CAN_Receive_IT_ExpectAndReturn(&hcan2, CAN_FIFO0, 1);
    HAL_GPIO_TogglePin_Expect(LED_RED_GPIO_Port, LED_RED_Pin);

    HAL_CAN_RxCpltCallback(&hcan2);


    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[0], auxBmsInputs[0], "The actual value is supposed to be 5");
    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[1], auxBmsInputs[1], "The actual value is supposed to be 6");
}

