//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
#include "stm32f4xx_hal.h"

#include "AuxStatus.h"
#include "OrionStatusHelper.h"
#include "AuxStatusHelper.h"
#include "UpdateChargeAllowanceTaskTest.h"

void setUp(void)
{
    setNominalOrionStatus();
    setNominalAuxStatus();
}

void tearDown(void)
{
    clearOrionStatus();
    clearAuxStatus();
}

int main (void)
{
    return runUpdateChargeAllowanceTaskTests();

}