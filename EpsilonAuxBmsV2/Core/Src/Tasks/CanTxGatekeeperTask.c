#include "CanTxGatekeeperTask.h"

void canTxGatekeeperTask(void* arg)
{
    CanTxGatekeeperQueueData canTxQueueData = (CanTxGatekeeperQueueData)
    {
        0
    };

    for (;;)
    {
        canTxGatekeeper(&canTxQueueData);
    }
}

/*
Transmits CAN messages that are put on canTxGatekeeperQueue if there are any mailboxes available.
It toggles the blue LED for every transmission.
*/
void canTxGatekeeper(CanTxGatekeeperQueueData* canTxQueueData)
{
    osMessageQueueGet(canTxGatekeeperQueue, canTxQueueData, NULL, osWaitForever);

    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0)
    {
        uint32_t mailbox;

        if (HAL_CAN_AddTxMessage(&hcan1, &(canTxQueueData->canTxHeader), canTxQueueData->data, &mailbox) == HAL_OK)
        {
            HAL_GPIO_TogglePin(BLU_LED_GPIO_Port, BLU_LED_Pin);
        }
    }
}
