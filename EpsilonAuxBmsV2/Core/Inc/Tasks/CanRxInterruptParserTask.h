#pragma once
#include "cmsis_os.h"
#include "CanRxQueueData.h"
#include "OrionCanInfo.h"

extern osMessageQueueId_t orionInterfaceQueue;
extern osMessageQueueId_t canRxParserQueue;

// CAN Rx STDIDs
static const uint32_t ORION_HIGH_LOW_VOLTAGES_STDID  = 0x305;
static const uint32_t ORION_TEMP_INFO_STDID = 0x304;
static const uint32_t ORION_PACK_INFO_STDID = 0x302;

void canRxInterruptParserTask(void* arg);
void canRxInterruptParser(OrionCanInfo* queueData, CanRxQueueData* canQueueData);
