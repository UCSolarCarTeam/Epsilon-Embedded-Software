#include "BlueLedToggleTask.h"

void blueLedToggleTask(void const* arg)
{
    //One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    osMutexId_t* canMutex = (osMutexId_t*)arg; // Get mutex that was passed as an argument

    for (;;)
    {
        osDelayUntil(prevWakeTime + 1);  //TODO: Replace 1 with time period for delay
        //TODO: Check blue toggle flag and toggle blue LED
        //TODO: Send CAN message indicating current state of LED
    }
}
