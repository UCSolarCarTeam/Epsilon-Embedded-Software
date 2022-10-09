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
    uint8_t voltageReceived = 0;
    uint8_t tempReceived = 0;
    uint8_t packReceived = 0;

    for (;;)
    {
        canRxInterruptParser(&orionQueueData, &canQueueData, &voltageReceived, &tempReceived, &packReceived);
    }
}

/*
Receives data from the CAN Rx Callback and parses it to be presented to the Orion Interface as an OrionCanInfo struct
If data is an Orion message, parse it and put it on orionInterfaceQueue as a CAN Rx Interrupt type message
Toggle green LED to indicate CAN message received
*/
void canRxInterruptParser(OrionCanInfo* orionQueueData, CanRxQueueData* canQueueData, uint8_t *voltageReceived, uint8_t *tempReceived, uint8_t *packReceived)
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
        vTracePrintF(lowCellVoltageTrace,"%dh", orionQueueData->lowCellVoltage * DEFAULT_VOLTAGE_UNITS);
        vTracePrintF(highCellVoltageTrace, "%dh", orionQueueData->highCellVoltage * DEFAULT_VOLTAGE_UNITS);
        orionMessageReceived = 1;
        *voltageReceived = 1;
    }
    else if (hdr.StdId == ORION_TEMP_INFO_STDID && hdr.DLC == 8)
    {
        orionQueueData->highTemperature = data[0];
        vTracePrintF(batteryTemperatureTrace, "%d", orionQueueData->highTemperature);
        orionMessageReceived = 1;
        *tempReceived = 1;
    }
    else if (hdr.StdId == ORION_PACK_INFO_STDID && hdr.DLC == 8)
    {
        short packCurrentInt =  // Units 0.1 A
            (data[0] << 0) |
            (data[1] << 8);
        orionQueueData->packCurrent = (float)packCurrentInt / 10.0f;
        vTracePrintF(packCurrentTrace, "%f", orionQueueData->packCurrent);
        orionMessageReceived = 1;
        *packReceived = 1;
    }
    else if (hdr.StdId == DRIVER_CONTROLS_LIGHTS_INPUTS) 
    {
        manualChargeTrip = data[0] >> 6; //1 means we trip it

    }

    if (orionMessageReceived && *voltageReceived && *tempReceived && *packReceived)
    {
        HAL_GPIO_TogglePin(GRN_LED_GPIO_Port, GRN_LED_Pin);
        osMessageQueuePut(orionInterfaceQueue, orionQueueData, 0, TASK_QUEUE_PUT_TIMEOUT);
    }
}
