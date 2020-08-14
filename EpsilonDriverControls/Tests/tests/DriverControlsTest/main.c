//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "DriverControlsTest.h"

CAN_HandleTypeDef hcan2;

void setUp(void)
{
    static CanTxMsgTypeDef txMessage;
    hcan2.pTxMsg = &txMessage;
    setAccelValuesQueue(0);
    setRegenValuesQueue(0);
}

void tearDown(void)
{

}

int main (void)
{
    UNITY_BEGIN();

    runDriverControlTests();

    return UNITY_END();
}