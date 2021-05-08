#include "unity.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockstm32f4xx_hal_can.h"
#include "Mockcmsis_os.h"
#include "Lights.h"
#include "LightsTest.h"

LightsInfo lightsInfo = {0};
RegenBrakeInfo regenBrakeInfo = {0};

uint8_t lightsInputs;
uint8_t auxBmsInputs[2];
uint8_t batteryErrors[5];
uint8_t driversInputs[4];
SigLightsHandle sigLightsHandle;

osMutexId* canHandleMutex;

void runLightsTests()
{
    RUN_TEST(test_updateLights_allLightsOff);
    RUN_TEST(test_updateLights_case2);
    RUN_TEST(test_updateLights_case3);
    RUN_TEST(test_blinkSignalLights_signalLightsInputsDisabled);
    RUN_TEST(test_blinkSignalLights_enableSignalLightsAfterPreviousStateWasDisabled);
    RUN_TEST(test_blinkSignalLights_enabledToEnabled_keepBlinkersOffAndResetBlinkerTimerToZero);
    RUN_TEST(test_blinkSignalLights_enabledToEnabled_turnBlinkersOnAndUpdateTimer);
    RUN_TEST(test_blinkSignalLights_enabledToEnabled_keepBlinkersOffAndUpdateTimer);
    RUN_TEST(test_updateStrobeLight_strobeLightOnAndUpdateBlinkerTimer);
    RUN_TEST(test_updateStrobeLight_strobeLightOffAndResetBlinkerTimer);
    RUN_TEST(test_updateStrobeLight_strobeLightOffAndUpdateBlinkerTimer);
    RUN_TEST(test_reportLightsToCan);
    RUN_TEST(test_sendHeartbeat);
    RUN_TEST(test_HAL_CAN_RxCpltCallback_lightsInput);
    RUN_TEST(test_HAL_CAN_RxCpltCallback_batteryStatErrors);
    RUN_TEST(test_HAL_CAN_RxCpltCallback_driversInputs);
    RUN_TEST(test_HAL_CAN_RxCpltCallback_auxBmsInput);
    RUN_TEST(test_HAL_CAN_RxCpltCallback_batteryStatErrorsANDToggleRedPin);
}

//TESTS for updateLights()

/*
Test updateLights(). Headlights, brakes, hazards and bms strobe are
off. Checks if lightsInfo.leftSignal matches sigLightsHandle.left and
lightsInto.rightSignal matches sigLightsHandle.right
*/
void test_updateLights_allLightsOff()
{
    uint32_t prevWakeTime = 0;
    lightsInputs = 0b00000001;
    driversInputs[1] = 0;
    driversInputs[2] = 0;
    auxBmsInputs[1] = 0;
    driversInputs[3] = 0b00000000;
    lightsInfo.leftSignal = 1;
    lightsInfo.rightSignal = 0;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
    updateLights(&lightsInfo, &prevWakeTime, &regenBrakeInfo);

    TEST_ASSERT_EQUAL_MESSAGE(lightsInfo.leftSignal, sigLightsHandle.left,
                              "sigLightsHandle.left expected to be 1");
    TEST_ASSERT_EQUAL_MESSAGE(lightsInfo.rightSignal, sigLightsHandle.right,
                              "sigLightsHandle.right expected to be 0");
}

/*
Test updateLights(). headLightsLow is on, headLightsHigh is off, breaklights, hazards, and bms strobe are
all on. checking to see if sigLightsHandle.left = LIGHT_ON and
sigLightsHAndle.right = LIGHTS_ON
*/
void test_updateLights_case2()
{
    uint32_t prevWakeTime = 0;
    lightsInputs = 0b00100010;
    driversInputs[3] = 0b00000001;
    driversInputs[1] = 0b00010000;
    driversInputs[2] = 0b00000001;
    auxBmsInputs[1] = 0b00000001;
    lightsInfo.leftSignal = 0;
    lightsInfo.rightSignal = 0;
    uint8_t headLightsHigh = 0;
    uint8_t headLightsLow = 1;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, headLightsHigh);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, headLightsLow);
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_ON);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
    updateLights(&lightsInfo, &prevWakeTime, &regenBrakeInfo);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.left,
                              "sigLightsHandle.left expected to be LIGHT_ON");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.right,
                              "sigLightsHandle.right expected to be LIGHT_ON");
}
/*
test updateLights().When leftSignal is off and rightSignal is off,
and breaklights, hazards and bms strobe are on. Checking to see that if
sigLightsHandle.left = LIGHTS_ON and sigLightsHandle.right = LIGHTS_ON
*/
void test_updateLights_case3()
{
    uint32_t prevWakeTime = 0;
    lightsInputs = 0b0100110;
    driversInputs[3] = 0b00000001;
    driversInputs[1] = 0b00010000;
    driversInputs[2] = 0b00000001;
    auxBmsInputs[1] = 0b00000001;
    lightsInfo.leftSignal = 0;
    lightsInfo.rightSignal = 0;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(HLOW_GPIO_Port, HLOW_Pin, LIGHT_ON);
    HAL_GPIO_WritePin_Expect(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_ON);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
    updateLights(&lightsInfo, &prevWakeTime, &regenBrakeInfo);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.left,
                              "sigLightsHandle.left expected to be LIGHT_ON");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHT_ON, sigLightsHandle.right,
                              "sigLightsHandle.right expected to be LIGHT_ON");
}

//TESTS for blinkSignalLights()

/*
test blinkSignalLights().Both signal lights are disabled. CHecking to see if
prevSigState is 0 or not
*/
void test_blinkSignalLights_signalLightsInputsDisabled()
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

/*
test blinkSignalLights. left signal and right signal are both on.
Checking to see if blinkerTimer becomes 0 and if prevSigState becomes 1
*/
void test_blinkSignalLights_enableSignalLightsAfterPreviousStateWasDisabled()
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


/*
test blinkSignalLights. Given that the left and right signals are both on and prevSigState is 1, 
the blinkTimer is set to be greater than the blinker frequency*2 (770), then the
blinkerTimer is expected to reset to Zero.
*/
void test_blinkSignalLights_enabledToEnabled_keepBlinkersOffAndResetBlinkerTimerToZero()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 1;
    sigLightsHandle.left = 1;
    sigLightsHandle.right = 1;

    uint32_t blinkerTimer = 771;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);

    TEST_ASSERT_EQUAL_MESSAGE(0, blinkerTimer, "blinkerTimer is supposed to be 0");
}

/*
test blinkSignalLights. Both left and right signals are on. Checking to see
if blinkerTimer is updated when both the signals are on so expected/updated blinkerTimer should be blinkerTimer + 10.
*/
void test_blinkSignalLights_enabledToEnabled_turnBlinkersOnAndUpdateTimer()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 1;
    sigLightsHandle.left = 1;
    sigLightsHandle.right = 1;

    uint32_t blinkerTimer = 300;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_ON);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_ON);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);

    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer, "blinkerTimer is supposed to be 310");
}
/*
test blinkSignalLights. Given that the left and right signals are both on and prevSigState is 1,
the blinkerTimer is set to be greater than the blinker frequency but less than double the frequency
(385 < blinkerTimer < 770). With thse conditions set, the expected outcome is for the blinkers to remain
off and the the blinkerTimer will be updated by the lights update frequency(10).
*/
void test_blinkSignalLights_enabledToEnabled_keepBlinkersOffAndUpdateTimer()
{
    uint32_t prevWakeTime = 0;
    uint8_t prevSigState = 1;
    sigLightsHandle.left = 1;
    sigLightsHandle.right = 1;

    uint32_t blinkerTimer = 400;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
    HAL_GPIO_WritePin_Expect(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
    blinkSignalLights(&prevWakeTime, &blinkerTimer, &prevSigState);

    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer, "blinkerTimer is supposed to be 410");
}

//TESTS for updateStrobeLight()

/*
test updateStrobeLight. Given that the auxBmsInputs signals the strobelight to
be on, and the blinkerTimer is less than the blinker frequency(385), the strobe light
is expected to remain on and the blinkerTimer is to be updated by the lights update frequency.
*/
void test_updateStrobeLight_strobeLightOnAndUpdateBlinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 300;
    char strobeLight;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;

    auxBmsInputs[1] = 0b00000001;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
    updateStrobeLight(&prevWakeTime, &blinkerTimer, &strobeLight);

    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer, "blinkerTimer is supposed to be 310");
}

/*
test updateStrobeLight. The STROBE is turned off. Checking to see if blinkerTimer
is resetted back to 0
*/
void test_updateStrobeLight_strobeLightOffAndResetBlinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 800;
    char strobeLight;
    uint32_t expectedBlinkerTimer = 0;
    auxBmsInputs[1] = 0b0;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
    updateStrobeLight(&prevWakeTime, &blinkerTimer, &strobeLight);

    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer,
                              "blinkerTimer is supposed to be 0");
}

/*
test updateStrobeLight. The STROBE is turned off. Checking to see if blinkerTimer
is updated to the new expected BlinkerTimer of 410.
*/
void test_updateStrobeLight_strobeLightOffAndUpdateBlinkerTimer()
{
    uint32_t prevWakeTime = 0;
    uint32_t blinkerTimer = 400;
    char strobeLight;
    uint32_t expectedBlinkerTimer = blinkerTimer + 10;
    auxBmsInputs[1] = 0b1;

    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_UPDATE_FREQ, 0);
    HAL_GPIO_WritePin_Expect(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
    updateStrobeLight(&prevWakeTime, &blinkerTimer, &strobeLight);

    TEST_ASSERT_EQUAL_MESSAGE(expectedBlinkerTimer, blinkerTimer,
                              "blinkerTimer is supposed to be 410");
}

//TEST for reportLightsToCan()

/*
test reportLightsToCan.
Checks to see if the lights data can be turned into a CAN message and be transmitted
*/
void test_reportLightsToCan()
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

// TEST for sendHeartbeat()

/*
For every heartbeat, the green light should be toggled and a CAN msg should be delivered with the
correct stdid for the heartbeat with a signal of 1.
*/
void test_sendHeartbeat()
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

//TESTS for HAL_CAN_RxCpltCallback()

/*
tests the function when the CAN message is directed to lights input and the message is
properly received
*/
void test_HAL_CAN_RxCpltCallback_lightsInput()
{
    hcan2.pRxMsg->StdId = LIGHTS_INPUT_STDID;
    hcan2.pRxMsg->DLC = 1;
    hcan2.pRxMsg->Data[0] = 2;

    HAL_CAN_Receive_IT_ExpectAndReturn(&hcan2, CAN_FIFO0, HAL_OK);
    HAL_CAN_RxCpltCallback(&hcan2);

    TEST_ASSERT_EQUAL_MESSAGE(hcan2.pRxMsg->Data[0], lightsInputs, "The actual value is supposed to be 2");
}

/*
tests when the CAN message is directed to battery stat errors and the message is
properly received
*/
void test_HAL_CAN_RxCpltCallback_batteryStatErrors()
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

/*
tests when the CAN message is directed to driversInputs and the message is
properly received
*/
void test_HAL_CAN_RxCpltCallback_driversInputs()
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

/*
tests when the CAN message is directed to the Aux BMS input and the message is
properly received
*/
void test_HAL_CAN_RxCpltCallback_auxBmsInput()
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

/*
tests when the CAN message is directed to battery stat errors and the message is
not properly received
*/
void test_HAL_CAN_RxCpltCallback_batteryStatErrorsANDToggleRedPin()
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
