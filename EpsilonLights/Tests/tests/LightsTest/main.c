//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "LightsTest.h"

CAN_HandleTypeDef hcan2;
void setUp(void)
{
    static CanTxMsgTypeDef txMessage;
    hcan2.pTxMsg = &txMessage;

    static CanRxMsgTypeDef rxMessage;
    hcan2.pRxMsg = &rxMessage;

    static CAN_TypeDef inst;
    hcan2.Instance = &inst;
}

void tearDown(void)
{

}

int main (void)
{
    UNITY_BEGIN();

    runLightsTests();

    return UNITY_END();
}