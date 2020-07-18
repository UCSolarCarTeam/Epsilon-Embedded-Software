#include <string.h>
#include "unity.h"

#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockcmsis_os.h"

#include "DriverControls.h"
#include "TestCaseA.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

CAN_HandleTypeDef hcan2;

osPoolId canPool;

osMessageQId canQueue;
uint8_t auxBmsInputs[2];
float   motor0VehicleVelocityInput;
float   motor1VehicleVelocityInput;

void runDriverControlTests()
{
    RUN_TEST(test_sendMusicTask);
}

void test_sendMusicTask()
{
    TEST_ASSERT_EQUAL(1, 1);
}
