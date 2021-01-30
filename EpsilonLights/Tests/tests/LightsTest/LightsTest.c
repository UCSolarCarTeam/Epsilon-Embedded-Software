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
RUN_TEST(test_headlightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff);
RUN_TEST(test_headlightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn);
}



void test_headlightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff()
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

void test_headlightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn()
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