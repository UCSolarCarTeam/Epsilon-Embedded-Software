#include "cmsis_os.h"

#include "MpptRtr.h"

void mpptRtrTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t channel = -1;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MPPT_HEARTBEAT_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        MpptCanMsg* msg = (MpptCanMsg*)osPoolAlloc(canTxPool);
        // Set Channel
        channel = (channel++) % 3;
        // Populate CAN Message
        msg->StdId = MPPT_STDID;
        msg->Channel = channel;
        // Send CAN Message
        osMessagePut(canTxQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendCanTask(void const* arg)
{
    for (;;)
    {
        osEvent evt = osMessageGet(canTxQueue, osWaitForever); // Blocks

        if (evt.status == osEventMessage)
        {
            MpptCanMsg* msg = (MpptCanMsg*)evt.value.p;
            // Populate CAN Message
            hcan1.pTxMsg->StdId = msg->StdId;
            hcan1.pTxMsg->DLC = MPPT_DLC;
            // Send CAN Message
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            HAL_CAN_Transmit_IT(&hcan1);
            // Deallocate CAN message
            osPoolFree(canTxPool, msg);
        }
    }
}