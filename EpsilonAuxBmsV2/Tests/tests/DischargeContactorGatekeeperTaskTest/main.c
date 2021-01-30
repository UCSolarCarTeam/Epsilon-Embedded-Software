//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "DischargeContactorGatekeeperTaskTest.h"


extern osEventFlagsId_t contactorControlEventBits;
extern AuxBmsContactorState auxBmsContactorState;

void setUp(void)
{
    contactorControlEventBits = 0;
    auxBmsContactorState = (AuxBmsContactorState)
    {
        0
    };
}

void tearDown(void)
{
}

int main (void)
{
    return runDischargeContactorGatekeeperTaskTest();
}
