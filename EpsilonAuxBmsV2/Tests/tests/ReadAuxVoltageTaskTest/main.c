//Include all files required for your functions to run
#include <stdio.h>
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "ReadAuxVoltageTaskTest.h"

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
    UNITY_BEGIN();

    runReadAuxVoltageTaskTests();

    return UNITY_END();
}
