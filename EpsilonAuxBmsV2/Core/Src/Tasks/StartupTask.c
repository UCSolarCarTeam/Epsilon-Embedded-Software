#include "StartupTask.h"

void startupTask(void* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        startup(&prevWakeTime);
    }
}

/*
The first task to execute when the processor starts.
Its purpose is to check that the precharge current is low, then trigger the execution of the Common Contactor Gatekeeper and exit.
If the precharge current isn’t low it will keep looping till it’s successful.
*/
void startup(uint32_t* prevWakeTime)
{
    if (isCurrentLow(0))
    {
        osEventFlagsSet(contactorControlEventBits, COMMON_CLOSED);
        auxBmsContactorState.commonState =  OPEN;
        osThreadExit();
    }
    else
    {
        auxBmsContactorState.commonState =  CONTACTOR_ERROR;
        *prevWakeTime += STARTUP_TASK_FREQ;
        osDelayUntil(*prevWakeTime);
    }
}
