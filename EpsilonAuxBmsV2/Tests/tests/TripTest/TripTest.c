#include "unity.h"
#include "TripTest.h"
#include "Trip.h"
#include "Mockstm32f4xx_hal_gpio.h"

extern OrionCanInfo message;
extern AuxTrip auxTripToUpdate;
uint8_t returnValue;

void runTripTests()
{
    RUN_TEST(test_updateAuxTripProtectionTrip);
    RUN_TEST(test_updateAuxTripAllDischargeTrip);
    RUN_TEST(test_updateAuxTripAllChargeTrip);
    RUN_TEST(test_checkDischargeTripDueToLowCell);
    RUN_TEST(test_checkDischargeTripDueToHighTempAndCurrent);
    RUN_TEST(test_checkDischargeTripDueToPackCurrent);
    RUN_TEST(test_checkDischargeTripForNoTrip);
    RUN_TEST(test_checkChargeTripDueToHighCell);
    RUN_TEST(test_checkChargeTripDueToHighTempAndCurrent);
    RUN_TEST(test_checkChargeTripDueToPackCurrent);
    RUN_TEST(test_checkChargeTripForNoTrip);
}

void test_updateAuxTripProtectionTrip()
{
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 1);
    message.packCurrent = -1;

    AuxTrip expectedAuxTripToUpdate = {
        .chargeTripDueToHighCellVoltage = 0,
        .chargeTripDueToHighTemperatureAndCurrent = 0,
        .chargeTripDueToPackCurrent = 0,
        .dischargeTripDueToLowCellVoltage = 0,
        .dischargeTripDueToHighTemperatureAndCurrent = 0,
        .dischargeTripDueToPackCurrent = 0,
        .protectionTrip = 1
    };

    updateAuxTrip(&message, &auxTripToUpdate);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.protectionTrip, auxTripToUpdate.protectionTrip, "checkProtectionTrip did not return a 1");
}

void test_updateAuxTripAllDischargeTrip()
{
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 0);
    message.lowCellVoltage = 25000;
    message.highTemperature = 60;
    message.packCurrent = 230;
    
    AuxTrip expectedAuxTripToUpdate = {
        .chargeTripDueToHighCellVoltage = 0,
        .chargeTripDueToHighTemperatureAndCurrent = 0,
        .chargeTripDueToPackCurrent = 0,
        .dischargeTripDueToLowCellVoltage = 1,
        .dischargeTripDueToHighTemperatureAndCurrent = 1,
        .dischargeTripDueToPackCurrent = 1,
        .protectionTrip = 0
    };

    updateAuxTrip(&message, &auxTripToUpdate);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.dischargeTripDueToLowCellVoltage, auxTripToUpdate.dischargeTripDueToLowCellVoltage, "dischargeTripDueToLowCellVoltage did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent, auxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent, "dischargeTripDueToHighTemperatureAndCurrent did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.dischargeTripDueToPackCurrent, auxTripToUpdate.dischargeTripDueToPackCurrent, "dischargeTripDueToPackCurrent did not return a 1");
}

void test_updateAuxTripAllChargeTrip()
{
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 0);
    message.highCellVoltage = 43000;
    message.highTemperature = 45;
    message.packCurrent = -48;
    
    AuxTrip expectedAuxTripToUpdate = {
        .chargeTripDueToHighCellVoltage = 1,
        .chargeTripDueToHighTemperatureAndCurrent = 1,
        .chargeTripDueToPackCurrent = 1,
        .dischargeTripDueToLowCellVoltage = 0,
        .dischargeTripDueToHighTemperatureAndCurrent = 0,
        .dischargeTripDueToPackCurrent = 0,
        .protectionTrip = 0
    };

    updateAuxTrip(&message, &auxTripToUpdate);
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.chargeTripDueToHighCellVoltage, auxTripToUpdate.chargeTripDueToHighCellVoltage, "chargeTripDueToHighCellVoltage did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent, auxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent, "chargeTripDueToHighTemperatureAndCurrent did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(expectedAuxTripToUpdate.chargeTripDueToPackCurrent, auxTripToUpdate.chargeTripDueToPackCurrent, "chargeTripDueToPackCurrent did not return a 1");
}


