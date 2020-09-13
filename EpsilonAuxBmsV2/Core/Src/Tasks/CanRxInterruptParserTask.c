#include "CanRxInterruptParserTask.h"

void canRxInterruptParserTask(void* arg)
{
    OrionCanInfo queueData = (OrionCanInfo)
    {
        0
    };

    for (;;)
    {
        canRxInterruptParser(&queueData);
    }
}

void canRxInterruptParser(OrionCanInfo* queueData)
{
    CanRxQueueData* canQueueData = NULL;
    osMessageQueueGet(canRxParserQueue, canQueueData, NULL, osWaitForever);

    if (canQueueData == NULL)
    {
        return;
    }

    CAN_RxHeaderTypeDef hdr = canQueueData->canRxHeader;
    uint8_t* data = canQueueData->data;
    uint8_t orionMessageReceived = 0;

    if (hdr.StdId == ORION_MAX_MIN_VOLTAGES_STDID && hdr.DLC == 8)
    {
        // Voltages are 2 bytes each, and memory is stored in little endian format
        queueData->minCellVoltage = (uint16_t)data[0] | data[1] << 8; // Min Cell voltage
        queueData->maxCellVoltage = (uint16_t)data[3] | data[4] << 8; // Max Cell Voltage
        orionMessageReceived = 1;
    }
    else if (hdr.StdId == ORION_TEMP_INFO_STDID && hdr.DLC == 8)
    {
        queueData->highTemperature = data[0];
        orionMessageReceived = 1;
    }
    else if (hdr.StdId == ORION_PACK_INFO_STDID && hdr.DLC == 8)
    {
        short packCurrentInt =  // Units 0.1 A
            (data[0] << 0) |
            (data[1] << 8);
        queueData->packCurrent = (float)packCurrentInt / 10.0f;
        orionMessageReceived = 1;
    }

    if (orionMessageReceived)
    {
        osMessageQueuePut(orionInterfaceQueue, queueData, 0, 0);
    }

    vPortFree(canQueueData);
}
