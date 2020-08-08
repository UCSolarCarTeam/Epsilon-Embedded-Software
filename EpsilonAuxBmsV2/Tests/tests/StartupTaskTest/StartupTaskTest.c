#include <string.h>
#include "unity.h"

#include "StartupTaskTest.h"
#include "Mockcmsis_os2.h"
#include "ContactorState.h"
#include "MockCheckCurrent.h"
#include "ContactorEventFlags.h"
#include "StartupTask.h"
osEventFlagsId_t contactorControlEventBits;

AuxBmsContactorState auxBmsContactorState;

void startupTaskCurrentLowTest()
{
    uint32_t prevWakeTime = 0;
    isCurrentLow_ExpectAndReturn(0, 1);
    osEventFlagsSet_ExpectAndReturn(contactorControlEventBits, COMMON_ON, 0);
    osThreadExit_Expect();
    startup(&prevWakeTime);
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, OPEN, "Common contactor state not OPEN");
}

void startupTaskCurrentHighTest()
{
    uint32_t prevWakeTime = 0;
    isCurrentLow_ExpectAndReturn(0, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + STARTUP_TASK_FREQ, osOK);
    startup(&prevWakeTime);
    TEST_ASSERT_EQUAL_MESSAGE(auxBmsContactorState.commonState, CONTACTOR_ERROR, "Common contactor state not ERROR");
}

int runStartupTaskTest()
{
    UNITY_BEGIN();

    RUN_TEST(startupTaskCurrentLowTest);
    RUN_TEST(startupTaskCurrentHighTest);

    return UNITY_END();
}
