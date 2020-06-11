//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"

#include "AuxStatus.h"
#include "OrionStatusHelper.h"
#include "SetContactorsTaskTest.h"

AuxStatus auxStatus;
ADC_HandleTypeDef hadc1;

void setUp(void)
{
}

void tearDown(void)
{
}

int main (void)
{
    UNITY_BEGIN();

    runSetContactorsTests();

    return UNITY_END();
}