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
    RUN_TEST(test_checkDischargeTripDueToLowCell);
    RUN_TEST(test_checkDischargeTripDueToHighTempAndCurrent);
    RUN_TEST(test_checkDischargeTripDueToPackCurrent);
    RUN_TEST(test_checkDischargeTripForNoTrip);
    RUN_TEST(test_checkChargeTripDueToHighCell);
    RUN_TEST(test_checkChargeTripDueToHighTempAndCurrent);
    RUN_TEST(test_checkChargeTripDueToPackCurrent);
    RUN_TEST(test_checkChargeTripForNoTrip);
    RUN_TEST(test_checkProtectionTripForTrip);
    RUN_TEST(test_checkProtectionTripForNoTrip);
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
    message.lowCellVoltage = 30000;
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
    message.highCellVoltage = 41000;
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

void test_checkDischargeTripDueToLowCell()
{
    message.lowCellVoltage = 25000;
    returnValue = checkDischargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkDischargeTrip did not return a 1");
}

void test_checkDischargeTripDueToHighTempAndCurrent()
{
    message.lowCellVoltage = 30000;
    message.highTemperature = 60;
    message.packCurrent = 1;
    returnValue = checkDischargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkDischargeTrip did not return a 1");
}

void test_checkDischargeTripDueToPackCurrent()
{
    message.highTemperature = 58;
    message.packCurrent = 230;
    returnValue = checkDischargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkDischargeTrip did not return a 1");
}

void test_checkDischargeTripForNoTrip()
{
    message.lowCellVoltage = 30000;
    message.packCurrent = 228;
    returnValue = checkDischargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(0, returnValue, "checkDischargeTrip did not return a 0");
}

void test_checkChargeTripDueToHighCell()
{
    message.highCellVoltage = 43000;
    message.highTemperature = 43;
    returnValue = checkChargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkChargeTrip did not return a 1");
}

void test_checkChargeTripDueToHighTempAndCurrent()
{
    message.highCellVoltage = 41000;
    message.highTemperature = 45;
    message.packCurrent = -1;
    returnValue = checkChargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkChargeTrip did not return a 1");
}

void test_checkChargeTripDueToPackCurrent()
{
    message.highTemperature = 43;
    message.packCurrent = -48;
    returnValue = checkChargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkChargeTrip did not return a 1");
}

void test_checkChargeTripForNoTrip()
{
    message.packCurrent = -46;
    returnValue = checkChargeTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(0, returnValue, "checkChargeTrip did not return a 0");
}

void test_checkProtectionTripForTrip()
{
    message.packCurrent = -1;
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 1);
    returnValue = checkProtectionTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkProtectionTrip did not return a 1");
}

void test_checkProtectionTripForNoTrip()
{
    message.packCurrent = 1;
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 0);
    returnValue = checkProtectionTrip(&message);
    TEST_ASSERT_EQUAL_MESSAGE(0, returnValue, "checkProtectionTrip did not return a 0");
}
