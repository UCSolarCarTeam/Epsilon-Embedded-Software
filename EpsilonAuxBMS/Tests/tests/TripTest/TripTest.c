#include "TripTest.h"
#include "OrionStatusHelper.h"
#include "unity.h"
#include "Trip.h"
#include "Mockstm32f4xx_hal_gpio.h"

void runTripTests()
{
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

    RUN_TEST(test_protectionTripping_tripWhenChargeContactorOffButCurrentNegative);
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

void test_protectionTripping_tripWhenChargeContactorOffButCurrentNegative()
{
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 1);

    orionStatus.packCurrent = -1;
    TEST_ASSERT_EQUAL_INT(1, protectionTripping());
}