#include <string.h>
#include "unity.h"

#include "ContactorStatusUpdateTaskTest.h"
#include "Mockcmsis_os2.h"
#include "MockCheckContactorError.h"
#include "ContactorState.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "ContactorStatusUpdateTask.h"
#include "AuxStatusHelper.h"

AuxBmsContactorState auxBmsContactorState;
osMutexId_t auxStatusContactorStatusUpdateMutex;
osThreadId_t commonContactorGatekeeperTaskHandle;
osThreadId_t chargeContactorGatekeeperTaskHandle;
osThreadId_t dischargeContactorGatekeeperTaskHandle;
osEventFlagsId_t contactorControlEventBits;

typedef struct
{
    GPIO_PinState commonSense;
    GPIO_PinState chargeSense;
    GPIO_PinState dischargeSense;
    GPIO_PinState hvEnable;
} SensePinValues;

void setDisconnectContactorExpects()
{
    osThreadSetPriority_ExpectAndReturn(chargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osThreadSetPriority_ExpectAndReturn(dischargeContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osThreadSetPriority_ExpectAndReturn(commonContactorGatekeeperTaskHandle, osPriorityRealtime, 0);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, COMMON_OPENED, 0);
}

void setSensePinReadValues(SensePinValues pinValues)
{
    HAL_GPIO_ReadPin_ExpectAndReturn(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin, pinValues.commonSense);
    HAL_GPIO_ReadPin_ExpectAndReturn(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin, pinValues.chargeSense);
    HAL_GPIO_ReadPin_ExpectAndReturn(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin, pinValues.dischargeSense);
    HAL_GPIO_ReadPin_ExpectAndReturn(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, pinValues.hvEnable);
}

void setContactorErrors(uint8_t commonError, uint8_t chargeError, uint8_t dischargeError, SensePinValues pinValues)
{
    // Inverted because source code uses active low
    isContactorError_ExpectAndReturn(!pinValues.chargeSense, auxBmsContactorState.chargeState, chargeError);
    isContactorError_ExpectAndReturn(!pinValues.dischargeSense, auxBmsContactorState.dischargeState, dischargeError);
    isContactorError_ExpectAndReturn(!pinValues.commonSense, auxBmsContactorState.commonState, commonError);
}

void basicAuxStatusUpdateTest()
{
    uint32_t prevWakeTime = 0;

    clearAuxStatus();

    SensePinValues pinValues = (SensePinValues)
    {
        .commonSense = GPIO_PIN_RESET,
         .chargeSense = GPIO_PIN_SET,
          .dischargeSense = GPIO_PIN_RESET,
           .hvEnable = GPIO_PIN_SET
    };
    setSensePinReadValues(pinValues);

    auxBmsContactorState.commonState = CLOSED;
    auxBmsContactorState.chargeState = OPEN;
    auxBmsContactorState.dischargeState = CLOSED;
    auxBmsContactorState.startupDone = 0;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.commonContactorState = 1;
    expectedAuxStatus.chargeContactorState = 0;
    expectedAuxStatus.dischargeContactorState = 1;
    expectedAuxStatus.highVoltageEnableState = 1;

    setContactorErrors(0, 0, 0, pinValues);
    osMutexAcquire_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + CONTACTOR_STATUS_UPDATE_FREQ, 0);
    contactorStatusUpdate(&prevWakeTime);
    assertAuxStatusEqual(expectedAuxStatus);
    TEST_ASSERT_EQUAL_MESSAGE(0, auxBmsContactorState.startupDone, "Startup unexpectedly done");
}

void basicInverseAuxStatusUpdateTest()
{
    uint32_t prevWakeTime = 0;

    clearAuxStatus();

    SensePinValues pinValues = (SensePinValues)
    {
        .commonSense = GPIO_PIN_SET,
         .chargeSense = GPIO_PIN_RESET,
          .dischargeSense = GPIO_PIN_SET,
           .hvEnable = GPIO_PIN_RESET
    };
    setSensePinReadValues(pinValues);

    auxBmsContactorState.commonState = OPEN;
    auxBmsContactorState.chargeState = CLOSED;
    auxBmsContactorState.dischargeState = OPEN;
    auxBmsContactorState.startupDone = 0;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.commonContactorState = 0;
    expectedAuxStatus.chargeContactorState = 1;
    expectedAuxStatus.dischargeContactorState = 0;
    expectedAuxStatus.highVoltageEnableState = 0;

    setContactorErrors(0, 0, 0, pinValues);
    osMutexAcquire_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + CONTACTOR_STATUS_UPDATE_FREQ, 0);
    contactorStatusUpdate(&prevWakeTime);
    assertAuxStatusEqual(expectedAuxStatus);
    TEST_ASSERT_EQUAL_MESSAGE(0, auxBmsContactorState.startupDone, "Startup unexpectedly done");
}

void contactorErrorsTest()
{
    uint32_t prevWakeTime = 0;

    clearAuxStatus();

    SensePinValues pinValues = (SensePinValues)
    {
        .commonSense = GPIO_PIN_RESET,
         .chargeSense = GPIO_PIN_SET,
          .dischargeSense = GPIO_PIN_SET,
           .hvEnable = GPIO_PIN_RESET
    };
    setSensePinReadValues(pinValues);

    auxBmsContactorState.commonState = CLOSED;
    auxBmsContactorState.chargeState = CLOSED;
    auxBmsContactorState.dischargeState = OPEN;
    auxBmsContactorState.startupDone = 0;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.commonContactorState = 1;
    expectedAuxStatus.chargeContactorState = 0;
    expectedAuxStatus.dischargeContactorState = 0;
    expectedAuxStatus.highVoltageEnableState = 0;
    expectedAuxStatus.chargeOpenButShouldBeClosed = 1;
    expectedAuxStatus.chargeContactorError = 1;
    expectedAuxStatus.commonContactorError = 1;

    setContactorErrors(1, 1, 0, pinValues);
    osMutexAcquire_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + CONTACTOR_STATUS_UPDATE_FREQ, 0);

    setDisconnectContactorExpects();
    contactorStatusUpdate(&prevWakeTime);
    assertAuxStatusEqual(expectedAuxStatus);
    TEST_ASSERT_EQUAL_MESSAGE(1, auxBmsContactorState.contactorsDisconnected, "Contactors not disconnected");
    TEST_ASSERT_EQUAL_MESSAGE(0, auxBmsContactorState.startupDone, "Startup unexpectedly done");
}

void startupDoneTest()
{
    uint32_t prevWakeTime = 0;

    clearAuxStatus();

    SensePinValues pinValues = (SensePinValues)
    {
        .commonSense = GPIO_PIN_RESET,
         .chargeSense = GPIO_PIN_RESET,
          .dischargeSense = GPIO_PIN_RESET,
           .hvEnable = GPIO_PIN_SET
    };
    setSensePinReadValues(pinValues);

    auxBmsContactorState.commonState = CLOSED;
    auxBmsContactorState.chargeState = CLOSED;
    auxBmsContactorState.dischargeState = CLOSED;
    auxBmsContactorState.startupDone = 0;

    AuxStatus expectedAuxStatus = auxStatus;
    expectedAuxStatus.commonContactorState = 1;
    expectedAuxStatus.chargeContactorState = 1;
    expectedAuxStatus.dischargeContactorState = 1;
    expectedAuxStatus.highVoltageEnableState = 1;

    setContactorErrors(0, 0, 0, pinValues);
    osMutexAcquire_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusContactorStatusUpdateMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + CONTACTOR_STATUS_UPDATE_FREQ, 0);
    contactorStatusUpdate(&prevWakeTime);
    assertAuxStatusEqual(expectedAuxStatus);
    TEST_ASSERT_EQUAL_MESSAGE(1, auxBmsContactorState.startupDone, "Startup not done");
}

int runContactorStatusUpdateTaskTest()
{
    UNITY_BEGIN();
    RUN_TEST(basicAuxStatusUpdateTest);
    RUN_TEST(basicInverseAuxStatusUpdateTest);
    RUN_TEST(contactorErrorsTest);
    RUN_TEST(startupDoneTest);

    return UNITY_END();
}
