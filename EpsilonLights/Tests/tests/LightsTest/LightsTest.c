#include "unity.h"

#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockstm32f4xx_hal_can.h"
#include "Mockstm32f4xx_hal_adc.h"
#include "Mockcmsis_os.h"

#include "Lights.h"
#include "LightsTest.h"

updateLights lightCharacteristics = {0};

uint8_t lightsInputs;
uint8_t auxBmsInputs[2];
uint8_t batteryErrors[5];
SigLightsHandle sigLightsHandle;


void runLightsTests()
{
RUN_TEST(test_headlightsOff);
}

void test_headlightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff()
{
    lightsInputs = 0b00000001;
    driversInputs[1] = 0;
    driverInputs[2] = 0;
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