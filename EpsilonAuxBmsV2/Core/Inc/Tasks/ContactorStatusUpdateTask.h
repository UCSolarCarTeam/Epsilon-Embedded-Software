#pragma once
#include "cmsis_os.h"
#include "ContactorState.h"
#include "ContactorEventFlags.h"
#include "DisconnectContactors.h"
#include "AuxStatus.h"
#include "main.h"
#include "CheckContactorError.h"

static const int CONTACTOR_STATUS_UPDATE_FREQ = 100; //100ms 10HZ
extern AuxBmsContactorState auxBmsContactorState;
extern osMutexId_t auxStatusContactorStatusUpdateMutex;
extern AuxStatus auxStatus;

void contactorStatusUpdateTask(void* arg);

void contactorStatusUpdate(uint32_t* prevWakeTime);
