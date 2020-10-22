//Include all files required for your functions to run
#include <stdio.h>
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "OrionInterfaceTest.h"

void setUp(void)
{
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