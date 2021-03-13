//Include all files required for your functions to run
#include <stdio.h>
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "OrionInterfaceTest.h"

extern OrionCanInfo message;
// extern AuxStatus auxStatus;
// extern AuxTrip auxTrip;
// extern AuxBmsContactorState auxBmsContactorState;
// extern osEventFlagsId_t contactorControlEventBits;

void setUp(void)
{
    message.highCellVoltage = 0;
    message.lowCellVoltage = 0;
    message.highTemperature = 0;
    message.packCurrent = 0;
}

void tearDown(void)
{
}

int main (void)
{
    UNITY_BEGIN();

    runOrionInterfaceTests();

    return UNITY_END();
}