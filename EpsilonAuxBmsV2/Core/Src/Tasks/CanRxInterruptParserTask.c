#include "CanRxInterruptParserTask.h"

void canRxInterruptParserTask(void* arg)
{
    OrionCanInfo orionQueueData = (OrionCanInfo)
    {
        0
    };
    CanRxQueueData canQueueData = (CanRxQueueData)
    {
        0
    };

    for (;;)
    {
        canRxInterruptParser(&orionQueueData, &canQueueData);
    }
}

/*
Receives data from the CAN Rx Callback and parses it to be presented to the Orion Interface as an OrionCanInfo struct
If data is an Orion message, parse it and put it on orionInterfaceQueue as a CAN Rx Interrupt type message
Toggle green LED to indicate CAN message received
*/
void canRxInterruptParser(OrionCanInfo* orionQueueData, CanRxQueueData* canQueueData)
{
    osMessageQueueGet(canRxParserQueue, canQueueData, NULL, osWaitForever);

    CAN_RxHeaderTypeDef hdr = canQueueData->canRxHeader;
    uint8_t* data = canQueueData->data;
    uint8_t orionMessageReceived = 0;

    if (hdr.StdId == ORION_HIGH_LOW_VOLTAGES_STDID && hdr.DLC == 8)
    {
        // Voltages are 2 bytes each, and memory is stored in little endian format
        orionQueueData->lowCellVoltage = (uint16_t)data[0] | data[1] << 8; // Min Cell voltage
        orionQueueData->highCellVoltage = (uint16_t)data[3] | data[4] << 8; // Max Cell Voltage
        orionMessageReceived = 1;
    }
    else if (hdr.StdId == ORION_TEMP_INFO_STDID && hdr.DLC == 8)
    {
        orionQueueData->highTemperature = data[0];
        orionMessageReceived = 1;
    }
    else if (hdr.StdId == ORION_PACK_INFO_STDID && hdr.DLC == 8)
    {
        short packCurrentInt =  // Units 0.1 A
            (data[0] << 0) |
            (data[1] << 8);
        orionQueueData->packCurrent = (float)packCurrentInt / 10.0f;
        orionMessageReceived = 1;
    }

    if (orionMessageReceived)
    {
        HAL_GPIO_TogglePin(GRN_LED_GPIO_Port, GRN_LED_Pin);
        osMessageQueuePut(orionInterfaceQueue, orionQueueData, 0, TASK_QUEUE_PUT_TIMEOUT);
    }
}
