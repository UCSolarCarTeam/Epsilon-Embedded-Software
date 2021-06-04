#pragma once
#include "cmsis_os.h"
#include "main.h"

void memoryDebugTask(void* arg);

static const unsigned int MEMORY_DEBUG_TASK_FREQ = 1000; // 1s

extern osThreadId_t startupTaskHandle;
extern osThreadId_t canRxInterruptParserTaskHandle;
extern osThreadId_t orionInterfaceTaskHandle;
extern osThreadId_t canTxGatekeeperTaskHandle;
extern osThreadId_t sendAuxStatusTaskHandle;
extern osThreadId_t sendAuxTripTaskHandle;
extern osThreadId_t sendHeartbeatTaskHandle;
extern osThreadId_t commonContactorGatekeeperTaskHandle;
extern osThreadId_t chargeContactorGatekeeperTaskHandle;
extern osThreadId_t dischargeContactorGatekeeperTaskHandle;
extern osThreadId_t contactorStatusUpdateTaskHandle;
extern osThreadId_t readAuxVoltageTaskHandle;
