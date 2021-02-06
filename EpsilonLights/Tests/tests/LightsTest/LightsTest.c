#include "unity.h"

#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockstm32f4xx_hal_can.h"
//#include "Mockstm32f4xx_hal_adc.h"
#include "Mockcmsis_os.h"

#include "Lights.h"
#include "LightsTest.h"

updateLights lightCharacteristics = {0};

uint8_t lightsInputs;
uint8_t auxBmsInputs[2];
uint8_t batteryErrors[5];
uint8_t driversInputs[4];
SigLightsHandle sigLightsHandle;

CAN_HandleTypeDef hcan2;

void runLightsTests()
{
RUN_TEST(test_headLightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff);
RUN_TEST(test_headLightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn);
RUN_TEST(test_headLightsHigh_headLights_Low_breakLightsOn_hazardsOn_bmsStrobeLightOn);
RUN_TEST(blinkSignalLights_sigLightsLeftOFF_sigLightsRightOFF);
RUN_TEST(blinkSignalLights_sigLightsLeftON_sigLightsRightON_prevSigState_Zero;
RUN_TEST(blinkSignalLights_prevSigState0_sigLightsLeftON_sigLightsRightON);
RUN_TEST(updateStrobeLights2_ESTROBEON_UpdateblinkerTimer);
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
    TEST_ASSERT_EQUAL_MESSAGE(lightCharacteristics.rightSignal,sigLightsHandle.right,
                                "sigLightsHandle.right expected to be 0");

}

void test_headLightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn()
{
    lightsInputs = 0b00100010;
    driversInputs[3] = 0b00000001;
    driversInputs[1] = 0b00010000;
    driversInputs[2] = 0b00000001; // gives 00010010- this is regenbrakeint = 18
    auxBmxInputs[1] = 0b00000001;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 0;
    lightCharacteristics.rightSignal = 0;

    //mocks
    //Headlights Low
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, LIGHT_ON);
    // Brake lights on
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);
    //bmsStrobe on
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);

    updateLights1(&lightCharacteristics, &prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.left,
                                "sigLightsHandle.left expected to be LIGHT_ON");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON,sigLightsHandle.right,
                                "sigLightsHandle.right expected to be LIGHT_ON");


}

void test_headLightsHigh_headLights_Low_breakLightsOn_hazardsOn_bmsStrobeLightOn()
{
    lightsInputs = 0b0010110;
    driversInputs[3] = 0b00000001;
    driversInputs[1] = 0b00010000;
    driversInputs[2] = 0b00000001; // gives 00010010- this is regenbrakeint = 18
    auxBmxInputs[1] = 0b00000001;
    uint32_t prevWakeTime = 0;
    lightCharacteristics.leftSignal = 0;
    lightCharacteristics.rightSignal = 0;

    //mocks
    //Headlights Low
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, lightsCharacteristics -> headlightsHigh);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, lightsCharacteristics -> headlightsLow);
    // Brake lights on
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);
    //bmsStrobe on
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);

    updateLights1(&lightCharacteristics, &prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.left,
                                "sigLightsHandle.left expected to be LIGHT_ON");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON,sigLightsHandle.right,
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

void keepBlinkersOFF_resetBlinkerTimerToZero(uint32_t prevWakeTime, uint8_t prevSigstate)
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

