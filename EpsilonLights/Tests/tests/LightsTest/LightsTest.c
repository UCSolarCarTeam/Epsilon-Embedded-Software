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

CAN_HandleTypeDef hcan2;
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
}

void hi()
{
    printf("\n\n\n\n\n hello \n");
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
    //when headlights r on
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);

    //brake lights r off
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);

    //hazards off

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
    driversInputs[2] = 0b00000001; // gives 00010010- this is regenbrakeint = 18
    auxBmsInputs[1] = 0b00000001;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 0;
    lightCharacteristics.rightSignal = 0;

    uint8_t headLightsHigh = 0;
    uint8_t headLightsLow = 1;

    //mocks
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
    driversInputs[2] = 0b00000001; // gives 00010010- this is regenbrakeint = 18
    auxBmsInputs[1] = 0b00000001;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 0;
    lightCharacteristics.rightSignal = 0;

    //mocks
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

//Tests for blinkSignalLIghts

void blinkSignalLights_sigLightsLeftOFF_sigLightsRightOFF()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 10;
    uint32_t blinkerTimer = 0;

    sigLightsHandle.left = 0; //to be OFF
    sigLightsHandle.right = 0; //to be OFF

    //remember the mocks have to be in order
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);

    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);

    TEST_ASSERT_EQUAL_MESSAGE(0, prevSigState, "prevSigState is supposed to be 0");


}

void blinkSignalLights_sigLightsLeftON_sigLightsRightON_prevSigState_Zero()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 0; //cuz cant be zero
    uint32_t blinkerTimer = 0;

    //in order to not go into the first if statement and go into the else if
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
    uint8_t prevSigState = 1; //to make the else if statement false

    //in order to not go into the first if statement and the else if statement anf go into else
    sigLightsHandle.left = 2;
    sigLightsHandle.right = 2;

    keepBlinkersOFF_resetBlinkerTimerToZero(prevWakeTime, prevSigState);
    turnBlinkersON_updateTimer(prevWakeTime, prevSigState);
    keepBlinkersOFF_updateTimer(prevWakeTime, prevSigState);

}

void keepBlinkersOFF_resetBlinkerTimerToZero(uint32_t prevWakeTime, uint8_t prevSigState)
{
    uint32_t blinkerTimer = 771; //385*2=770

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);
    TEST_ASSERT_EQUAL_MESSAGE(0, blinkerTimer, "blinkerTimer is supposed to be 0");

}

void turnBlinkersON_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState)
{
    uint32_t blinkerTimer = 300; //any value which is less than BLINKER_FREQ
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


// TESTS FOR updateStrobeLight2

void updateStrobeLights2_ESTROBEON_UpdateblinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 300; //to go into the first if statement
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

// tests for reportLightsToCan
void reportLightsToCan_test()
{
    uint32_t prevWakeTime = 0;
    uint32_t expectedValue = 57;



    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_STATUS_FREQ, 0);

    osMutexWait_ExpectAndReturn(canHandleMutex, 0, osOK);
    HAL_GPIO_TogglePin_Expect(LED_BLUE_GPIO_Port, LED_BLUE_Pin);


    HAL_GPIO_ReadPin_ExpectAndReturn(HLOW_GPIO_Port, HLOW_Pin, 5);
    HAL_GPIO_ReadPin_ExpectAndReturn(HHIGH_GPIO_Port, HHIGH_Pin, 4);
    HAL_GPIO_ReadPin_ExpectAndReturn(BRAKE_GPIO_Port, BRAKE_Pin, 3);
    HAL_GPIO_ReadPin_ExpectAndReturn(LSIGNAL_GPIO_Port, LSIGNAL_Pin, 2);
    HAL_GPIO_ReadPin_ExpectAndReturn(RSIGNAL_GPIO_Port, RSIGNAL_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(ESTROBE_GPIO_Port, ESTROBE_Pin, 0);

    HAL_CAN_Transmit_IT_ExpectAndReturn(&hcan2, 0);
    osMutexRelease_ExpectAndReturn(canHandleMutex, 0);

    reportLightsToCan(&prevWakeTime, canHandleMutex);

    TEST_ASSERT_EQUAL_MESSAGE(expectedValue, hcan2.pTxMsg->Data[0], "expected value is supposed to be 57");

}

void sendHeartbeat_test()
{
    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_HEARTBEAT_FREQ, 0);
    osMutexWait_ExpectAndReturn(canHandleMutex, 0, osOK);
    HAL_GPIO_TogglePin_Expect(LED_GREEN_GPIO_Port, LED_GREEN_Pin);


    sendHeartbeat(canHandleMutex, &prevWakeTime);
    HAL_CAN_Transmit_IT_ExpectAndReturn(&hcan2, 0);
    osMutexRelease_ExpectAndReturn(canHandleMutex, 0);

    //TEST_ASSERT_EQUAL_MESSAGE(LIGHTS_HEARTBEAT_STDID, hcan2.pTxMsg->StdId, "expected to be LIGHTS_HEARTBEAT_STDID value");
    //TEST_ASSERT_EQUAL_MESSAGE(1, hcan2.pTxMsg->Data[0], "hcan2.pTxMsg->Data[0] should be 1");
}

/*void HAL_CAN_RxCpltCallback_StdI_test()
{
    hcan2->pRxMsg->Stdid = LIGHTS_INPUT_STDID;
    hcan2->pRxMsg->DLC = 1;
    hcan2->pRxMsg->Data[0] = 2;

    __HAL_CAN_CLEAR_FLAG_(hcan, CAN_FLAG_FMP0);



    TEST_ASSERT_EQUAL_MESSAGE(hcan2->pRxMsg->Data[0],lightsInputs, "The actual value is supposed to be 2");



}*/
