#include "OrionStatusHelper.h"
#include "unity.h"

void clearOrionStatus()
{
    orionStatus = (OrionStatus)
    {
        0
    };
}

void setNominalOrionStatus()
{
    orionStatus = (OrionStatus)
    {
        .batteryVoltagesInRange = 1,
         .chargeContactorOverriden = 0,
          .dischargeContactorOverriden = 0,
           .allowCharge = 1,
            .allowDischarge = 1,
             .shutOff = 0,
              .canMsgReceived = 0,
               .maxCellVoltage = 30000,
                .minCellVoltage = 30000,
                 .highTemperature = 20,
                  .packCurrent = 20
    };
}

void assertOrionStatusEqual(OrionStatus expected)
{
    TEST_ASSERT_EQUAL_MESSAGE(expected.batteryVoltagesInRange,
                              orionStatus.batteryVoltagesInRange,
                              "Check orionStatus.batteryVoltagesInRange");
    TEST_ASSERT_EQUAL_MESSAGE(expected.chargeContactorOverriden,
                              orionStatus.chargeContactorOverriden,
                              "Check orionStatus.chargeContactorOverriden");
    TEST_ASSERT_EQUAL_MESSAGE(expected.dischargeContactorOverriden,
                              orionStatus.dischargeContactorOverriden,
                              "Check orionStatus.dischargeContactorOverriden");
    TEST_ASSERT_EQUAL_MESSAGE(expected.allowCharge,
                              orionStatus.allowCharge,
                              "Check orionStatus.allowCharge");
    TEST_ASSERT_EQUAL_MESSAGE(expected.allowDischarge,
                              orionStatus.allowDischarge,
                              "Check orionStatus.allowDischarge");
    TEST_ASSERT_EQUAL_MESSAGE(expected.shutOff,
                              orionStatus.shutOff,
                              "Check orionStatus.shutOff");
    TEST_ASSERT_EQUAL_MESSAGE(expected.canMsgReceived,
                              orionStatus.canMsgReceived,
                              "Check orionStatus.canMsgReceived");
    TEST_ASSERT_EQUAL_MESSAGE(expected.maxCellVoltage,
                              orionStatus.maxCellVoltage,
                              "Check orionStatus.maxCellVoltage");
    TEST_ASSERT_EQUAL_MESSAGE(expected.minCellVoltage,
                              orionStatus.minCellVoltage,
                              "Check orionStatus.minCellVoltage");
    TEST_ASSERT_EQUAL_MESSAGE(expected.highTemperature,
                              orionStatus.highTemperature,
                              "Check orionStatus.highTemperature");
    TEST_ASSERT_EQUAL_MESSAGE(expected.packCurrent,
                              orionStatus.packCurrent,
                              "Check orionStatus.packCurrent");
    TEST_ASSERT_EQUAL_MESSAGE(expected.orionStatusMutex,
                              orionStatus.orionStatusMutex,
                              "Check orionStatus.orionStatusMutex");
}