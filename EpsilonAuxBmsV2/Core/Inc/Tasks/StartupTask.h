#pragma once
#include "cmsis_os.h"
#include "CheckCurrent.h"
#include "ContactorState.h"
#include "ContactorEventFlags.h"

static const unsigned int STARTUP_TASK_FREQ = 10; // 100 Hz

void startupTask(void* arg);
void startup(uint32_t* prevWakeTime);

extern osEventFlagsId_t contactorControlEventBits;
extern AuxBmsContactorState auxBmsContactorState;
