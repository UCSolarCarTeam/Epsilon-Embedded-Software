#include "cmsis_os.h"

#include "MpptRtr.h"

void mpptRtrTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t channel = CHANNEL_ZERO;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MPPT_HEARTBEAT_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        MpptCanMsg* msg = (MpptCanMsg*)osPoolAlloc(canTxPool);
        // Populate CAN Message
        msg->stdId = MPPT_STDID;
        msg->channel = channel;
        msg->dlc = MPPT_DLC;

        // Update Channel
        switch(channel)
        {
            case CHANNEL_ZERO:
                channel = CHANNEL_ONE;
            case CHANNEL_ONE:
                channel = CHANNEL_TWO;
            case CHANNEL_TWO:
                channel = CHANNEL_ZERO;
        }
        // Send CAN Message
        osMessagePut(canTxQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendMpptRtrCanTask(void const* arg)
{
    for (;;)
    {
        osEvent evt = osMessageGet(canTxQueue, osWaitForever); // Blocks

        if (evt.status == osEventMessage)
        {
            MpptCanMsg* msg = (MpptCanMsg*)evt.value.p;
            // Populate CAN Message
            hcan1.pTxMsg->StdId = msg->stdId;
            hcan1.pTxMsg->DLC = msg->dlc;
            // Send CAN Message
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            HAL_CAN_Transmit_IT(&hcan1);
            // Deallocate CAN message
            osPoolFree(canTxPool, msg);
        }
    }
}