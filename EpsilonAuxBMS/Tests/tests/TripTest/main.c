//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"

#include "OrionStatus.h"
#include "TripTest.h"

void setUp(void)
{
    clearOrionStatus();
}

void tearDown(void)
{
}

int main (void)
{
    UNITY_BEGIN();

    runTripTests();

    return UNITY_END();
}