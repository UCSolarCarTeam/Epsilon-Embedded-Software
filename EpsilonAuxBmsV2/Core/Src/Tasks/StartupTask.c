#include "StartupTask.h"

void startupTask(void* arg)
{
    uint32_t prevWakeTime = xTaskGetTickCount();
    uint32_t startUpCounter = 0;
    OrionCanInfo message = (OrionCanInfo)
    {
        0
    };

    for (;;)
    {
        startup(&prevWakeTime, &message, &startUpCounter);
    }
}

/*
The first task to execute when the processor starts.
Its purpose is to check that the precharge current is low, then trigger the execution of the Common Contactor Gatekeeper and exit.
If the precharge current isn’t low it will keep looping till it’s successful.
*/
void startup(uint32_t* prevWakeTime, OrionCanInfo *message, uint32_t* startUpCounter)
{
    auxBmsContactorState.orionHappyForStartup = !checkIfOrionGood(message, startUpCounter); // 1 means that it should disconnect contactor origionally so orion is not happy if it's one. 
    if(auxBmsContactorState.orionHappyForStartup) {
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
}
