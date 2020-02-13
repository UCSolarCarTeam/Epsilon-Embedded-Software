#include "TripTest.h"
#include "OrionStatus.h"
#include "unity.h"
#include "Trip.h"

OrionStatus orionStatus;

void clearOrionStatus()
{
    orionStatus = (OrionStatus)
    {
        0
    };
}

void test_chargeShouldTripFromHighCellVoltage()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.maxCellVoltage = 55555;
    TEST_ASSERT_EQUAL_INT(1, chargeShouldTrip());
}

void test_chargeShouldTripDuetoHighTempAndChargingCurrent()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.highTemperature = 45;
    orionStatus.packCurrent = -0.1f;
    TEST_ASSERT_EQUAL_INT(1, chargeShouldTrip());
}

void test_chargeShouldNotTripDuetoOnlyHighTemp()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.highTemperature = 45;
    orionStatus.packCurrent = 0.f;
    TEST_ASSERT_EQUAL_INT(0, chargeShouldTrip());
}

void test_chargeShouldNotTripIfNoCanMsgReceived()
{
    orionStatus.canMsgReceived = 0;
    orionStatus.highTemperature = 45;
    orionStatus.packCurrent = -0.1f;
    orionStatus.maxCellVoltage = 55555;
    TEST_ASSERT_EQUAL_INT(0, chargeShouldTrip());
}

void test_chargeShouldTripIfChargingCurrentIsTooHigh()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.packCurrent = -47.f;
    TEST_ASSERT_EQUAL_INT(1, chargeShouldTrip());
}

void test_dischargeShouldNotTripIfNoCanMsgReceived()
{
    orionStatus.canMsgReceived = 0;
    orionStatus.minCellVoltage = 25499;
    orionStatus.packCurrent = 230.f;
    orionStatus.highTemperature = 60;
    TEST_ASSERT_EQUAL_INT(0, dischargeShouldTrip());
}

void test_dischargeShouldTripIfMinCellVoltageTooLow()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.minCellVoltage = 25499;
    TEST_ASSERT_EQUAL_INT(1, dischargeShouldTrip());
}

void test_dischargeShouldTripIfTempTooHighAndCarIsDischarging()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.minCellVoltage = 25501;
    orionStatus.highTemperature = 60;
    orionStatus.packCurrent = 0.1f;
    TEST_ASSERT_EQUAL_INT(1, dischargeShouldTrip());
}

void test_dischargeShouldNotTripIfTempTooHighAndCarIsNotDischarging()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.minCellVoltage = 25501;
    orionStatus.highTemperature = 60;
    orionStatus.packCurrent = 0.f;
    TEST_ASSERT_EQUAL_INT(0, dischargeShouldTrip());
}

void test_dischargeShouldTripDueToPackCurrentTooHigh()
{
    orionStatus.canMsgReceived = 1;
    orionStatus.minCellVoltage = 25501;
    orionStatus.packCurrent = 230.f;
    TEST_ASSERT_EQUAL_INT(1, dischargeShouldTrip());
}