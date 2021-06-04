#include "AuxStatusHelper.h"
#include "unity.h"

void clearAuxStatus()
{
    auxStatus = (AuxStatus)
    {
        0
    };
}

void setNominalAuxStatus()
{
    auxStatus = (AuxStatus)
    {
        .commonContactorState = 1,
         .chargeContactorState = 1,
          .dischargeContactorState = 1,
           .auxVoltage = 1,
            .highVoltageEnableState = 1,
             .strobeBmsLight = 0,
              .allowCharge = 1,
               .allowDischarge = 1,
                .orionCanReceivedRecently = 1,
                 .chargeContactorError = 0,
                  .dischargeContactorError = 0,
                   .commonContactorError = 0,
                    .dischargeShouldTrip = 0,
                     .chargeShouldTrip = 0,
                      .chargeOpenButShouldBeClosed = 0,
                       .dischargeOpenButShouldBeClosed = 0
    };
}

void assertAuxStatusEqual(AuxStatus expected)
{
    TEST_ASSERT_EQUAL_MESSAGE(expected.commonContactorState,
                              auxStatus.commonContactorState,
                              "Check auxStatus.commonContactorState");
    TEST_ASSERT_EQUAL_MESSAGE(expected.chargeContactorState,
                              auxStatus.chargeContactorState,
                              "Check auxStatus.chargeContactorState");
    TEST_ASSERT_EQUAL_MESSAGE(expected.dischargeContactorState,
                              auxStatus.dischargeContactorState,
                              "Check auxStatus.dischargeContactorState");

    TEST_ASSERT_EQUAL_MESSAGE(expected.auxVoltage,
                              auxStatus.auxVoltage,
                              "Check auxStatus.auxVoltage");
    TEST_ASSERT_EQUAL_MESSAGE(expected.highVoltageEnableState,
                              auxStatus.highVoltageEnableState,
                              "Check auxStatus.highVoltageEnableState");
    TEST_ASSERT_EQUAL_MESSAGE(expected.strobeBmsLight,
                              auxStatus.strobeBmsLight,
                              "Check auxStatus.strobeBmsLight");
    TEST_ASSERT_EQUAL_MESSAGE(expected.allowCharge,
                              auxStatus.allowCharge,
                              "Check auxStatus.allowCharge");
    TEST_ASSERT_EQUAL_MESSAGE(expected.allowDischarge,
                              auxStatus.allowDischarge,
                              "Check auxStatus.allowDischarge");
    TEST_ASSERT_EQUAL_MESSAGE(expected.orionCanReceivedRecently,
                              auxStatus.orionCanReceivedRecently,
                              "Check auxStatus.orionCanReceivedRecently");
    TEST_ASSERT_EQUAL_MESSAGE(expected.chargeContactorError,
                              auxStatus.chargeContactorError,
                              "Check auxStatus.chargeContactorError");
    TEST_ASSERT_EQUAL_MESSAGE(expected.commonContactorError,
                              auxStatus.commonContactorError,
                              "Check auxStatus.commonContactorError");
    TEST_ASSERT_EQUAL_MESSAGE(expected.dischargeShouldTrip,
                              auxStatus.dischargeShouldTrip,
                              "Check auxStatus.dischargeShouldTrip");
    TEST_ASSERT_EQUAL_MESSAGE(expected.chargeShouldTrip,
                              auxStatus.chargeShouldTrip,
                              "Check auxStatus.chargeShouldTrip");
    TEST_ASSERT_EQUAL_MESSAGE(expected.chargeOpenButShouldBeClosed,
                              auxStatus.chargeOpenButShouldBeClosed,
                              "Check auxStatus.chargeOpenButShouldBeClosed");
    TEST_ASSERT_EQUAL_MESSAGE(expected.dischargeOpenButShouldBeClosed,
                              auxStatus.dischargeOpenButShouldBeClosed,
                              "Check auxStatus.dischargeOpenButShouldBeClosed");



}
