#include "cmsis_os.h"

#include "MpptRtr.h"

void mpptRtrTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t channel = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MPPT_HEARTBEAT_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        MpptCanMsg* msg = (MpptCanMsg*)osPoolAlloc(mpptCanTxPool);
        // Populate CAN Message
        msg->dlc = MPPT_DLC;
        msg->rtr = MPPT_RTR;

        // Update Channel
        switch (channel)
        {
            case 0:
                msg->stdId = MPPT_CHANNEL_ZERO_STDID;
                break;

            case 1:
                msg->stdId = MPPT_CHANNEL_ONE_STDID;
                break;

            case 2:
                msg->stdId = MPPT_CHANNEL_TWO_STDID;
                break;

            default: //Shouldn't get here
                channel = 0;
                break;
        }

        channel = (channel + 1) % 3;
        // Send CAN Message
        osMessagePut(mpptCanTxQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendMpptRtrCanTask(void const* arg)
{
    for (;;)
    {
        osEvent evt = osMessageGet(mpptCanTxQueue, osWaitForever); // Blocks

        if (evt.status == osEventMessage)
        {
            MpptCanMsg* msg = (MpptCanMsg*)evt.value.p;
            // Populate CAN Message
            hcan1.pTxMsg->StdId = msg->stdId;
            hcan1.pTxMsg->DLC = msg->dlc;
            hcan1.pTxMsg->RTR = msg->rtr;
            // Send CAN Message
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            HAL_CAN_Transmit_IT(&hcan1);
            // Deallocate CAN message
            osPoolFree(mpptCanTxPool, msg);
        }
    }
}
