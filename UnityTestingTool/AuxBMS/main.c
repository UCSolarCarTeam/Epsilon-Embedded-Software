//Include all files required for your functions to run
#include <stdio.h>

//Include required .h file for unity
#include "unity.h"
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

    //Tripping Tests
    RUN_TEST(test_chargeShouldTripFromHighCellVoltage);
    RUN_TEST(test_chargeShouldTripDuetoHighTempAndChargingCurrent);
    RUN_TEST(test_chargeShouldNotTripDuetoOnlyHighTemp);
    RUN_TEST(test_chargeShouldNotTripIfNoCanMsgReceived);
    RUN_TEST(test_chargeShouldTripIfChargingCurrentIsTooHigh);

    RUN_TEST(test_dischargeShouldNotTripIfNoCanMsgReceived);
    RUN_TEST(test_dischargeShouldTripIfMinCellVoltageTooLow);
    RUN_TEST(test_dischargeShouldTripIfTempTooHighAndCarIsDischarging);
    RUN_TEST(test_dischargeShouldNotTripIfTempTooHighAndCarIsNotDischarging);
    RUN_TEST(test_dischargeShouldTripDueToPackCurrentTooHigh);

    return UNITY_END();

}