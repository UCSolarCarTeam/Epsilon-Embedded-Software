//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"

#include "OrionStatus.h"
#include "AuxStatus.h"
#include "TripTest.h"
#include "SetContactorsTaskTest.h"

ADC_HandleTypeDef hadc1;
AuxStatus auxStatus;
CAN_HandleTypeDef hcan1;

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
    runSetContactorsTests();

    return UNITY_END();
}