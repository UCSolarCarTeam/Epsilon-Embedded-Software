#include "cmsis_os.h"

#include "MpptRtr.h"

/*Send the RTR request for the MPPTs*/
void mpptRtrTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t channel = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MPPT_HEARTBEAT_CAN_FREQ * 10);

        // Populate CAN Message
        switch (channel)
        {
            case 0:
                hcan2.pTxMsg->StdId = MPPT_CHANNEL_ZERO_STDID;
                break;

            case 1:
                hcan2.pTxMsg->StdId = MPPT_CHANNEL_ONE_STDID;
                break;

            case 2:
                hcan2.pTxMsg->StdId = MPPT_CHANNEL_TWO_STDID;
                break;

            default: //Shouldn't get here
                channel = 0;
                break;
        }

        hcan2.pTxMsg->DLC = MPPT_DLC;
        hcan2.pTxMsg->RTR = MPPT_RTR;

        // Send CAN Message
        if(HAL_CAN_Transmit_IT(&hcan2) == HAL_OK)
        {
          HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
        }

        // Update Channel
        channel = (channel + 1) % 3;
    }
}
