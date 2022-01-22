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
           .highVoltageEnableState = 1,
            .auxVoltage = 1,
             .strobeBmsLight = 0,
              .allowCharge = 1,
               .contactorError = 0,
                .startUpSequenceDone = 1,
                 .auxStatusMutex = NULL
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
    TEST_ASSERT_EQUAL_MESSAGE(expected.highVoltageEnableState,
                              auxStatus.highVoltageEnableState,
                              "Check auxStatus.highVoltageEnableState");
    TEST_ASSERT_EQUAL_MESSAGE(expected.auxVoltage,
                              auxStatus.auxVoltage,
                              "Check auxStatus.auxVoltage");
    TEST_ASSERT_EQUAL_MESSAGE(expected.strobeBmsLight,
                              auxStatus.strobeBmsLight,
                              "Check auxStatus.strobeBmsLight");
    TEST_ASSERT_EQUAL_MESSAGE(expected.allowCharge,
                              auxStatus.allowCharge,
                              "Check auxStatus.allowCharge");
    TEST_ASSERT_EQUAL_MESSAGE(expected.contactorError,
                              auxStatus.contactorError,
                              "Check auxStatus.contactorError");
    TEST_ASSERT_EQUAL_MESSAGE(expected.startUpSequenceDone,
                              auxStatus.startUpSequenceDone,
                              "Check auxStatus.startUpSequenceDone");
    TEST_ASSERT_EQUAL_MESSAGE(expected.auxStatusMutex,
                              auxStatus.auxStatusMutex,
                              "Check auxStatus.auxStatusMutex");
}