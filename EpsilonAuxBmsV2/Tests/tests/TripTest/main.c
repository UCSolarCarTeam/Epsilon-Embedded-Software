//Include all files required for your functions to run
#include <stdio.h>
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "TripTest.h"
#include "AuxTrip.h"
#include "OrionCanInfo.h"

AuxTrip auxTripToUpdate;
OrionCanInfo message;

void setUp(void)
{
    auxTripToUpdate.chargeTripDueToHighCellVoltage = 0;
    auxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent = 0;
    auxTripToUpdate.chargeTripDueToPackCurrent = 0;
    auxTripToUpdate.dischargeTripDueToLowCellVoltage = 0;
    auxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent = 0;
    auxTripToUpdate.dischargeTripDueToPackCurrent = 0;
    auxTripToUpdate.protectionTrip = 0;

    message.highCellVoltage = 0;
    message.lowCellVoltage = 0;
    message.highTemperature = 0;
    message.packCurrent = 0;
}

void tearDown(void)
{
}

int main(void)
{
    UNITY_BEGIN();

    runTripTests();

    return UNITY_END();
}