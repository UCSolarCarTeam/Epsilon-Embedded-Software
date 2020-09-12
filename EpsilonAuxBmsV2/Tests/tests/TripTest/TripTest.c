#include "unity.h"
#include "TripTest.h"
#include "Trip.h"
#include "Mockstm32f4xx_hal_gpio.h"

OrionCanInfo message = {0};
AuxTrip auxTripToUpdate = {0};
uint8_t returnValue;

void runTripTests()
{
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

void test_checkDischargeTripDueToLowCell()
{
    message.minCellVoltage = 25000;
    returnValue = checkDischargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkDischargeTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.dischargeTripDueToLowCellVoltage,
                              "auxTripToUpdate.dischargeTripDueToLowCellVoltage value is not 1");
}

void test_checkDischargeTripDueToHighTempAndCurrent()
{
    message.minCellVoltage = 30000;
    message.highTemperature = 60;
    message.packCurrent = 1;
    returnValue = checkDischargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkDischargeTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent,
                              "auxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent is not 1");
}

void test_checkDischargeTripDueToPackCurrent()
{
    message.highTemperature = 58;
    message.packCurrent = 230;
    returnValue = checkDischargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkDischargeTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.dischargeTripDueToPackCurrent,
                              "auxTripToUpdate.dischargeTripDueToPackCurrent is not 1");
}

void test_checkDischargeTripForNoTrip()
{
    message.minCellVoltage = 30000;
    message.packCurrent = 228;
    returnValue = checkDischargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(0, returnValue, "checkDischargeTrip did not return a 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.dischargeTripDueToLowCellVoltage,
                              "auxTripToUpdate.dischargeTripDueToLowCellVoltage is not 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent,
                              "auxTripToUpdate.dischargeTripDueToHighTemperatureAndCurrent is not 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.dischargeTripDueToPackCurrent,
                              "auxTripToUpdate.dischargeTripDueToPackCurrent is not 0");
}

void test_checkChargeTripDueToHighCell()
{
    message.maxCellVoltage = 43000;
    message.highTemperature = 43;
    returnValue = checkChargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkChargeTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.chargeTripDueToHighCellVoltage,
                              "auxTripToUpdate.chargeTripDueToHighCellVoltage is not 1");
}

void test_checkChargeTripDueToHighTempAndCurrent()
{
    message.maxCellVoltage = 41000;
    message.highTemperature = 45;
    message.packCurrent = -1;
    returnValue = checkChargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkChargeTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent,
                              "auxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent is not 1");
}

void test_checkChargeTripDueToPackCurrent()
{
    message.highTemperature = 43;
    message.packCurrent = -48;
    returnValue = checkChargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkChargeTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.chargeTripDueToPackCurrent,
                              "auxTripToUpdate.chargeTripDueToPackCurrent is not 1");
}

void test_checkChargeTripForNoTrip()
{
    message.packCurrent = -46;
    returnValue = checkChargeTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(0, returnValue, "checkChargeTrip did not return a 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.chargeTripDueToHighCellVoltage,
                              "auxTripToUpdate.chargeTripDueToHighCellVoltage is not 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent,
                              "auxTripToUpdate.chargeTripDueToHighTemperatureAndCurrent is not 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.chargeTripDueToPackCurrent,
                              "auxTripToUpdate.chargeTripDueToPackCurrent is not 0");
}

void test_checkProtectionTripForTrip()
{
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 1);
    returnValue = checkProtectionTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(1, returnValue, "checkProtectionTrip did not return a 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, auxTripToUpdate.protectionTrip,
                              "auxTripToUpdate.protectionTrip is not 1");
}

void test_checkProtectionTripForNoTrip()
{
    message.packCurrent = 1;
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, 0);
    returnValue = checkProtectionTrip(&message, &auxTripToUpdate);

    TEST_ASSERT_EQUAL_MESSAGE(0, returnValue, "checkProtectionTrip did not return a 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxTripToUpdate.protectionTrip,
                              "auxTripToUpdate.protectionTrip is not 0");
}
