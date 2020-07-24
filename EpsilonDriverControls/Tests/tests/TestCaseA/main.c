//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "TestCaseA.h"


void setUp(void)
{
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