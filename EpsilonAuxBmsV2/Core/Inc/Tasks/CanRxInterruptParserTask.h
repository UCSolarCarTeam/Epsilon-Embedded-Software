#pragma once
#include "cmsis_os.h"
#include "CanRxQueueData.h"
#include "OrionCanInfo.h"

extern osMessageQueueId_t orionInterfaceQueue;
extern osMessageQueueId_t canRxParserQueue;
extern uint32_t manualChargeTrip;

// CAN Rx STDIDs
static const uint32_t ORION_HIGH_LOW_VOLTAGES_STDID  = 0x305;
static const uint32_t ORION_TEMP_INFO_STDID = 0x304;
static const uint32_t ORION_PACK_INFO_STDID = 0x302;
static const uint32_t DRIVER_CONTROLS_LIGHTS_INPUTS = 0x701;

void canRxInterruptParserTask(void* arg);
void canRxInterruptParser(OrionCanInfo* orionQueueData, CanRxQueueData* canQueueData, uint8_t *voltageReceived, uint8_t *tempReceived, uint8_t *packReceived);
