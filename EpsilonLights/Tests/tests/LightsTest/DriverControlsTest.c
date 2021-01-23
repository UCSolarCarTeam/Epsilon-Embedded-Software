#include "unity.h"

#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockstm32f4xx_hal_can.h"
#include "Mockstm32f4xx_hal_adc.h"
#include "Mockcmsis_os.h"

#include "DriverControls.h"
#include "DriverControlsTest.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

osPoolId canPool;
osMessageQId canQueue;

uint8_t auxBmsInputs[2];
float   motor0VehicleVelocityInput;
float   motor1VehicleVelocityInput;

void checkDriveMsgStdidAndDlcForDriveCommands(CanMsg* expectedMotorDriveMsg);
void checkPowerMessageForDriveCommands(CanMsg* expectedMotorPowerMsg);
void expectedCanMessage(CanMsg* expectedCanMessage);
void setForwardReverseAndBrakePins(uint8_t setPins[]);

void runDriverControlTests()
{
    RUN_TEST(test_sendHeartbeat);
    RUN_TEST(test_sendLights);
    RUN_TEST(test_sendMusic);
    RUN_TEST(test_sendDriver);
    RUN_TEST(test_sendCan_EqualToHalOk);
    RUN_TEST(test_sendCan_NotEqualToHalOk);
    RUN_TEST(test_sendDriveCommands_wasPreviouslyResetAndWantToResetAgain);
    RUN_TEST(test_sendDriveCommands_wasNotPreviouslyResetAndWantToReset);
    RUN_TEST(test_sendDriveCommands_noResetsAtAll);
    RUN_TEST(test_sendDriveCommands_forwardAndReverseIsAnErrorState);
    RUN_TEST(test_sendDriveCommands_forwardAndMotorVehicleVelocityIsNotSafe);
    RUN_TEST(test_sendDriveCommands_reverseAndMotorVehicleVelocityIsNotSafe);
    RUN_TEST(test_sendDriveCommands_regenPercentageGreaterThanNonZeroThresholdAndAllowingCharge);
    RUN_TEST(test_sendDriveCommands_reverseAndAccelPercentageGreaterThanNonZeroThreshold);
    RUN_TEST(test_sendDriveCommands_forwardAndAccelPercentageGreaterThanNonZeroThreshold);
    RUN_TEST(test_sendDriveCommands_regenPercentageGreaterThanNonZeroThresholdAndNotAllowingCharge);
    RUN_TEST(test_sendDriveCommands_brake);
    RUN_TEST(test_sendDriveCommands_regenBrakeForwardAndReverseOff);
}

//test sendHeartbeat(). Checks msg->StdId, msg->DLC, msg->Data[0].
void test_sendHeartbeat()
{
    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, HEARTBEAT_CAN_FREQ, 0);
    CanMsg expectedMsg = {};
    expectCanMessage(&expectedMsg);

    sendHeartbeat(&prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(HEARTBEAT_STDID, expectedMsg.StdId,
                              "Expected heartbeat msg.StdID not HEARTBEAT_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(HEARTBEAT_DLC, expectedMsg.DLC,
                              "Expected heartbeat msg.DLC not HEARTBEAT_DLC");
    TEST_ASSERT_EQUAL_MESSAGE(1, expectedMsg.Data[0],
                              "Expected heartbeat msg.Data[0] not 1");
}

//Test sendLights(). Checks msg->StdId, msg->DLC, msg->Data[0].
//Set readpins to return 1010011 respectively. Data[0] calucates to 0x1A.
void test_sendLights()
{
    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, LIGHTS_CAN_FREQ, 0);
    CanMsg expectedMsg = {};
    expectCanMessage(&expectedMsg);

    HAL_GPIO_ReadPin_ExpectAndReturn(HEADLIGHTS_OFF_GPIO_Port, HEADLIGHTS_OFF_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(HEADLIGHTS_LOW_GPIO_Port, HEADLIGHTS_LOW_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(HEADLIGHTS_HIGH_GPIO_Port, HEADLIGHTS_HIGH_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(RSIGNAL_GPIO_Port, RSIGNAL_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(LSIGNAL_GPIO_Port, LSIGNAL_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(HAZARDS_GPIO_Port, HAZARDS_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(INTERIOR_GPIO_Port, INTERIOR_Pin, 1);

    sendLights(&prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(LIGHTS_STDID, expectedMsg.StdId,
                              "Expected lights msg.StdID not LIGHTS_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(LIGHTS_DLC, expectedMsg.DLC,
                              "Expected lights msg.DLC not LIGHTS_DLC");
    TEST_ASSERT_EQUAL_MESSAGE(0x1A, expectedMsg.Data[0],
                              "Expected lights msg.Data[0] not 0x1A");
}

//Test sendMusic(). Checks msg->StdId, msg->DLC, msg->Data[0].
//Set readpins to return 1010 respectively. Data[0] calculates to be 0xA.
void test_sendMusic()
{
    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, MUSIC_CAN_FREQ, 0);
    CanMsg expectedMsg = {};
    expectCanMessage(&expectedMsg);

    HAL_GPIO_ReadPin_ExpectAndReturn(VOLUME_UP_GPIO_Port, VOLUME_UP_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(VOLUME_DOWN_GPIO_Port, VOLUME_DOWN_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(NEXT_SONG_GPIO_Port, NEXT_SONG_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(LAST_SONG_GPIO_Port, LAST_SONG_Pin, 0);

    sendMusic(&prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(MUSIC_STDID, expectedMsg.StdId,
                              "Expected music msg.StdID not MUSIC_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(MUSIC_DLC, expectedMsg.DLC,
                              "Expected music msg.DLC not MUSIC_DLC");
    TEST_ASSERT_EQUAL_MESSAGE(0xA, expectedMsg.Data[0],
                              "Expected music msg.Data[0] not 0xA");
}

//Test sendMusic(). Checks msg->StdId, DLC, Data[0], Data[1], Data[2], Data[3].
//Set readpins to return 01110100 respectively.
//Data[0]: 0x3E
//Data[1]: 0x80
//Data[2]: 0x02
//Data[3]: 0xD1
void test_sendDriver()
{
    regenValuesQueue[0] = 10;
    regenValuesQueue[1] = 40;
    regenValuesQueue[2] = 70;
    regenValuesQueue[3] = 20;
    regenValuesQueue[4] = 60;

    accelValuesQueue[0] = 40;
    accelValuesQueue[1] = 80;
    accelValuesQueue[2] = 30;
    accelValuesQueue[3] = 60;
    accelValuesQueue[4] = 100;

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVER_CAN_FREQ, 0);
    CanMsg expectedMsg = {};
    expectCanMessage(&expectedMsg);

    HAL_GPIO_ReadPin_ExpectAndReturn(BRAKES_GPIO_Port, BRAKES_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(FORWARD_GPIO_Port, FORWARD_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(REVERSE_GPIO_Port, REVERSE_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(PUSH_TO_TALK_GPIO_Port, PUSH_TO_TALK_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(HORN_GPIO_Port, HORN_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(RESET_GPIO_Port, RESET_Pin, 1);
    HAL_GPIO_ReadPin_ExpectAndReturn(AUX_GPIO_Port, AUX_Pin, 0);
    HAL_GPIO_ReadPin_ExpectAndReturn(LAP_GPIO_PORT, LAP_PIN, 0);

    sendDriver(&prevWakeTime);

    TEST_ASSERT_EQUAL_MESSAGE(DRIVER_STDID, expectedMsg.StdId,
                              "Expected driver msg.StdID not DRIVER_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(DRIVER_DLC, expectedMsg.DLC,
                              "Expected driver msg.DLC not DRIVER_DLC");
    uint8_t expectedMsgData[8] = {0x3E, 0x80, 0x02, 0xD1};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMsg.Data, 4,
                                          "Expected msg.Data context or size incorrect");
}

//Test sendCan() when HAL_CAN_Transmit_IT(&hcan2) == HAL_OK.
//Checks hcan2.pTxMsg->StdId, DLC, Data[0] and if red pin toggled.
void test_sendCan_EqualToHalOk()
{
    CanMsg heartbeatMsg;
    memset(heartbeatMsg.Data, 0, 1);
    heartbeatMsg.StdId = HEARTBEAT_STDID;
    heartbeatMsg.DLC = HEARTBEAT_DLC;
    heartbeatMsg.Data[0] = 1;

    osEvent expectedEvt;
    expectedEvt.status = 0x10;
    expectedEvt.value.p = &heartbeatMsg;

    osMessageGet_ExpectAndReturn(canQueue, osWaitForever, expectedEvt);
    osPoolFree_ExpectAndReturn(canPool, &heartbeatMsg, osOK);

    HAL_CAN_Transmit_IT_ExpectAndReturn(&hcan2, HAL_OK);
    HAL_GPIO_TogglePin_Expect(LED_RED_GPIO_Port, LED_RED_Pin);
    sendCan();

    TEST_ASSERT_EQUAL_MESSAGE(HEARTBEAT_STDID, hcan2.pTxMsg->StdId,
                              "Expected heartbeat hcan.pTxMsg->StdId not HEARTBEAT_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(HEARTBEAT_DLC, hcan2.pTxMsg->DLC,
                              "Expected heartbeat hcan.pTxMsg->DLC not HEARTBEAT_DLC");
    TEST_ASSERT_EQUAL_MESSAGE(heartbeatMsg.Data[0], hcan2.pTxMsg->Data[0],
                              "Expected heartbeat hcan.pTxMsg->Data[0] not 1");
}

//Test sendCan() when HAL_CAN_Transmit_IT(&hcan2) != HAL_OK
//Checks hcan2.pTxMsg->StdId, DLC, Data[0] and if HAL_CAN_Init(&hcan2) is called
void test_sendCan_NotEqualToHalOk()
{
    CanMsg heartbeatMsg;
    memset(heartbeatMsg.Data, 0, 1);
    heartbeatMsg.StdId = HEARTBEAT_STDID;
    heartbeatMsg.DLC = HEARTBEAT_DLC;
    heartbeatMsg.Data[0] = 1;

    osEvent expectedEvt;
    expectedEvt.status = 0x10;
    expectedEvt.value.p = &heartbeatMsg;

    osMessageGet_ExpectAndReturn(canQueue, osWaitForever, expectedEvt);
    osPoolFree_ExpectAndReturn(canPool, &heartbeatMsg, osOK);

    HAL_CAN_Transmit_IT_ExpectAndReturn(&hcan2, HAL_ERROR);
    HAL_CAN_Init_ExpectAndReturn(&hcan2, HAL_OK);
    sendCan();

    TEST_ASSERT_EQUAL_MESSAGE(HEARTBEAT_STDID, hcan2.pTxMsg->StdId,
                              "Expected heartbeat hcan.pTxMsg->StdId not HEARTBEAT_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(HEARTBEAT_DLC, hcan2.pTxMsg->DLC,
                              "Expected heartbeat hcan.pTxMsg->DLC not HEARTBEAT_DLC");
    TEST_ASSERT_EQUAL_MESSAGE(heartbeatMsg.Data[0], hcan2.pTxMsg->Data[0],
                              "Expected heartbeat hcan.pTxMsg->Data[0] not 1");
}

//Test sendDriveCommands() when prevResetStatus is true and reset pin is set.
//Check that prevResetStatus == true and no reset CAN message has been sent.
void test_sendDriveCommands_wasPreviouslyResetAndWantToResetAgain()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 1,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {1, 1, 1, 0};
    setForwardReverseAndBrakePins(setPins);

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);
    TEST_ASSERT_EQUAL_MESSAGE(1, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");

}

//Test sendDriveCommands() when prevResetStatus is false and reset pin is set.
//Chheck that prevResetStatus == true and reset CAN message sent.
//Check that reset CAN msg->StdId = MOTOR_RESET_STDID
void test_sendDriveCommands_wasNotPreviouslyResetAndWantToReset()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {1, 1, 1, 0};
    setForwardReverseAndBrakePins(setPins);

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    CanMsg expectedResetMsg = {};
    expectCanMessage(&expectedResetMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);
    TEST_ASSERT_EQUAL_MESSAGE(MOTOR_RESET_STDID, expectedResetMsg.StdId,
                              "Expected reset msg.StdId not MOTOR_RESET_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(1, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");

}

//Test sendDriveCommands() when prevResetStatus is false and reset pin is not set.
//Check that prevResetStatus == false and no reset CAN message is sent.
void test_sendDriveCommands_noResetsAtAll()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {1, 1, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//test sendDriveCommands() when forward and reverse are both true.
//Check that motorCurrentOut == 0 and motorVelocityOut == 0
void test_sendDriveCommands_forwardAndReverseIsAnErrorState()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {0, 0, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//Test sendDriveCommands() when forward is true and motor0//1VehicleVelocityInputs are
//less than SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD (-1.38)
//Check that motorCurrentOut == 0 and motorVelocityOut == 0
void test_sendDriveCommands_forwardAndMotorVehicleVelocityIsNotSafe()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {0, 1, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    //less than SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD
    motor0VehicleVelocityInput = -3.0f;
    motor1VehicleVelocityInput = -3.0f;

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");

}

//Test sendDriveCommands() when reverse is true and motor0//1VehicleVelocityInputs are
//greater than SAFE_VEHICLE_VELOCITY_TO_GO_REVERSE (1.38)
//Check that motorCurrentOut == 0 and motorVelocityOut == 0
void test_sendDriveCommands_reverseAndMotorVehicleVelocityIsNotSafe()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {1, 0, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    //greater than SAFE_VEHICLE_VELOCITY_TO_GO_REVERSE
    motor0VehicleVelocityInput = 2.0f;
    motor1VehicleVelocityInput = 2.0f;

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");

}

//Test sendDriveCommands() when regenPrecentage is greater than NON_ZERO_THRESHOLD (0.17) and
//allowCharge is true.
//Set forward and motor0//1VehicleVelocityInput > SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD so that isNewDirectionSafe will return 1.
//Set auxBmsInputs[1] = 0x02 so that allowCharge is true
//Set all indices of regenValuesQueue to be 71 and set newRegen = 0.
//Then, regenPercentage == 0.56 and so motorCurrentOut == 0.028708335 (0x3CEB2DBE) (https://www.h-schmidt.net//FloatConverter//IEEE754.html)
//Check motorCurrentOut == 0.028708335 and motorVelocityOut == 0
void test_sendDriveCommands_regenPercentageGreaterThanNonZeroThresholdAndAllowingCharge()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 1, //so that regenPercentage will be 0.56
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);

    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 0); //regenPercentage will result in 0.56
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    //so that isNewDirectionSafe will return 1
    uint8_t setPins[4] = {0, 1, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    //so that isNewDirectionSafe will return 1
    motor0VehicleVelocityInput = 3.0f;
    motor1VehicleVelocityInput = 3.0f;

    //regenPercentage = 0.56
    setRegenValuesQueue(71);

    setAccelValuesQueue(0);

    auxBmsInputs[1] = 0x02;

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);

    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0.03975f, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0.03975f");
    TEST_ASSERT_EQUAL_MESSAGE(2, expectedDriveCommandsInfo.regenQueueIndex,
                              "Expected driveCommandsInfo.regenQueueIndex not 2");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    //driveCommandsInfo->motorCurrentOut = 0.028708335
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0xBE, 0x2D, 0xEB, 0x3C};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//Test sendDriveCommands() when regenPrecentage is greater than NON_ZERO_THRESHOLD (0.17) and
//allowCharge is false.
//Set forward and motor0//1VehicleVelocityInput > SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD so that isNewDirectionSafe will return 1.
//Set auxBmsInputs[1] = 0x0 so that allowCharge is false
//Check motorCurrentOut == 0 and motorVelocity == 0
void test_sendDriveCommands_regenPercentageGreaterThanNonZeroThresholdAndNotAllowingCharge()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 10, //To ensure that the regenPercentage will be 0.71
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);

    //Weird  unity issue: whenthe HAL_ADC_PollForConversion is commented,
    //it will cause the forward and accel percentage greater than 0
    //as well as the reverse and accel percentage greater 0 to fail.
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, 1000);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    //so that isNewDirectionSafe will return 1
    uint8_t setPins[4] = {0, 1, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    //so that isNewDirectionSafe will return 1
    motor0VehicleVelocityInput = 3.0f;
    motor1VehicleVelocityInput = 3.0f;

    //regenPercentage = 0.71
    setRegenValuesQueue(71);
    setAccelValuesQueue(0);

    auxBmsInputs[1] = 0x0;

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);

    TEST_ASSERT_EQUAL_MESSAGE(1, expectedDriveCommandsInfo.regenQueueIndex,
                              "Expected driveComandsInfo.regenQueueIndex not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//Test sendDriveCommands() when brake is true.
//Set forward and motor0//1VehicleVelocityInput > SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD so that isNewDirectionSafe will return 1.
//Check that motorCurrentOut == 0 and motorVelocityOut == 0.
void test_sendDriveCommands_brake()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {0, 1, 0, 1};
    setForwardReverseAndBrakePins(setPins);

    //so that isNewDirectionSafe will return 1
    motor0VehicleVelocityInput = 3.0f;
    motor1VehicleVelocityInput = 3.0f;

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);

    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//Test sendDriveCommands() when forward is true and accelPercentage is greater than NON_ZERO_THRESHOLD (0.17).
//Set motor0//1VehicleVelocityInput > SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD so that isNewDirectionSafe will return 1.
//Set all indices of accelValuesQueue to be 71 and set newAccel = 0.
//Then, accelPercentage == 0.56 and so motorCurrentOut == 0.05741667 (0x3D6B2DBE)
//Check that motorCurrentOut == 0.05741667 and motorVelocityOut == MAX_FORWARD_RPM (20000).
void test_sendDriveCommands_forwardAndAccelPercentageGreaterThanNonZeroThreshold()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 1,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);

    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_OK);

    HAL_ADC_GetValue_ExpectAndReturn(&hadc2, 0);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {0, 1, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    //so that isNewDirectionSafe will return 1
    motor0VehicleVelocityInput = 3.0f;
    motor1VehicleVelocityInput = 3.0f;

    setRegenValuesQueue(0);
    //accelPercentage = 0.56
    setAccelValuesQueue(71);

    HAL_GPIO_TogglePin_Expect(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);

    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");
    TEST_ASSERT_EQUAL_MESSAGE(2, expectedDriveCommandsInfo.accelQueueIndex,
                              "Expected driveCommandsInfo.accelQueueIndex not 2");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    // MAX_Forward_RPM = 20000
    //driveCommandsInfo->motorCurrentOut = 0.05741667
    uint8_t expectedMsgData[8] = {0x00, 0x40, 0x9C, 0x46, 0xBE, 0x2D, 0x6B, 0x3D};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//Test sendDriveCommands when reverse is true and accelPercentage is greater than NON_ZERO_THRESHOLD (0.17)
//Set motor0//1VehicleVelocityInput < SAFE_VEHICLE_VELOCITY_TO_GO_REVERSE so that isNewDirectionSafe will return 1.
//Set all indices of accelValuesQueue to be 71 and set newAccel = 0.
//Then, accelPercentage == 0.56 and so motorCurrentOut == 0.05741667 (0x3D6B2DBE)
//Check that motorCurrentOut == 0.05741667 and motorVelocityOut == MAX_FORWARD_RPM (20000).
void test_sendDriveCommands_reverseAndAccelPercentageGreaterThanNonZeroThreshold()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 1,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);

    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_OK);

    HAL_ADC_GetValue_ExpectAndReturn(&hadc2, 0);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {1, 0, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    //so that isNewDirectionSafe will return 1
    motor0VehicleVelocityInput = 0.0f;
    motor1VehicleVelocityInput = 0.0f;

    setRegenValuesQueue(0);

    //accelPercentage = 0.56
    setAccelValuesQueue(71);

    HAL_GPIO_TogglePin_Expect(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);

    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");
    TEST_ASSERT_EQUAL_MESSAGE(2, expectedDriveCommandsInfo.accelQueueIndex,
                              "Expected driveCommandsInfo.accelQueueIndex not 2");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    // MAX_REVERSE_RPM = -20000
    //driveCommandsInfo->motorCurrentOut = 0.05741667
    uint8_t expectedMsgData[8] = {0x00, 0x40, 0x9C, 0xC6, 0xBE, 0x2D, 0x6B, 0x3D};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//Test sendDriveCommands when regen, brake, forward and reverse are all false.
//Check that motorCurrentOut == 0 and motorVelocityOut == 0s
void test_sendDriveCommands_regenBrakeForwardAndReverseOff()
{
    DriveCommandsInfo expectedDriveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t prevWakeTime = 0;
    osDelayUntil_ExpectAndReturn(&prevWakeTime, DRIVE_COMMANDS_FREQ, 0);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_ERROR);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc2, ADC_POLL_TIMEOUT, HAL_ERROR);

    //setPins = {forward, reverse, brakes, reset}
    uint8_t setPins[4] = {1, 1, 1, 1};
    setForwardReverseAndBrakePins(setPins);

    motor0VehicleVelocityInput = 0.0f;
    motor1VehicleVelocityInput = 0.0f;

    setRegenValuesQueue(0);
    setAccelValuesQueue(0);

    CanMsg expectedMotorDriveMsg = {};
    expectCanMessage(&expectedMotorDriveMsg);

    CanMsg expectedMotorPowerMsg = {};
    expectCanMessage(&expectedMotorPowerMsg);

    sendDriveCommands(&prevWakeTime, &expectedDriveCommandsInfo);

    checkDriveMsgStdidAndDlcForDriveCommands(&expectedMotorDriveMsg);
    checkPowerMessageForDriveCommands(&expectedMotorPowerMsg);

    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.prevResetStatus,
                              "Expected driveCommandsInfo.prevResetStatus not 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, expectedDriveCommandsInfo.motorCurrentOut,
                              "Expected driveCommandsInfo.motorCurrentOut not 0");

    //memcpy copies 8 bytes of data into msg.data and msg.data stores 8 bits of data in each index.
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorDriveMsg.Data, 8,
                                          "Expected motor drive msg.Data context or size incorrect");
}

//helper funciton to check the motor drive CAN msg->StdId and DLC.
void checkDriveMsgStdidAndDlcForDriveCommands(CanMsg* expectedMotorDriveMsg)
{
    TEST_ASSERT_EQUAL_MESSAGE(MOTOR_DRIVE_STDID, expectedMotorDriveMsg->StdId,
                              "Expected motor drive msg.StdId not MOTOR_DRIVE_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(MOTOR_DRIVE_DLC, expectedMotorDriveMsg->DLC,
                              "Expected motor drive msg.DLC not MOTOR_DRIVE_DLC");
}

//helper function to check motor power CAN message things as they do not change.
void checkPowerMessageForDriveCommands(CanMsg* expectedMotorPowerMsg)
{
    uint8_t expectedMsgData[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x3f};
    TEST_ASSERT_EQUAL_MESSAGE(MOTOR_POWER_STDID, expectedMotorPowerMsg->StdId,
                              "Expected motor power msg.StdId not MOTOR_POWER_STDID");
    TEST_ASSERT_EQUAL_MESSAGE(MOTOR_POWER_DLC, expectedMotorPowerMsg->DLC,
                              "Expected motor power msg.DLC not MOTOR_POWER_DLC");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedMsgData, expectedMotorPowerMsg->Data, 8,
                                          "Expected motor power msg.Data context or size incorrect");
}

//helper function to set regen values queue
void setRegenValuesQueue(uint8_t number)
{
    for (int i = 0; i < REGEN_QUEUE_SIZE; i++)
    {
        regenValuesQueue[i] = number;
    }
}

//helper function to set accel values queue
void setAccelValuesQueue(uint8_t number)
{
    for (int i = 0; i < ACCEL_QUEUE_SIZE; i++)
    {
        accelValuesQueue[i] = number;
    }
}

//helper function for osPoolAlloc and osMessagePut expect and returns for CAN message
void expectCanMessage(CanMsg* expectedCanMessage)
{
    osPoolAlloc_ExpectAndReturn(canPool, expectedCanMessage);
    osMessagePut_ExpectAndReturn(canQueue, (uint32_t)expectedCanMessage, osWaitForever, osOK);
}

//helper funciton to set the pins
void setForwardReverseAndBrakePins(uint8_t setPins[])
{
    HAL_GPIO_ReadPin_ExpectAndReturn(FORWARD_GPIO_Port, FORWARD_Pin, setPins[0]);
    HAL_GPIO_ReadPin_ExpectAndReturn(REVERSE_GPIO_Port, REVERSE_Pin, setPins[1]);
    HAL_GPIO_ReadPin_ExpectAndReturn(BRAKES_GPIO_Port, BRAKES_Pin, setPins[2]);
    HAL_GPIO_ReadPin_ExpectAndReturn(RESET_GPIO_Port, RESET_Pin, setPins[3]);
}
