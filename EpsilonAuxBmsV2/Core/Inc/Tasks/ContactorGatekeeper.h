#pragma once
#include "cmsis_os.h"
#include "CheckCurrent.h"
#include "ContactorState.h"
#include "ContactorEventFlags.h"
#include "main.h"

extern osEventFlagsId_t contactorControlEventBits;
extern AuxBmsContactorState auxBmsContactorState;

static const unsigned int COMMON_CHECK_CONTACTOR_DELAY = 1000; // 1s
static const unsigned int COMMON_RETRY_CONTACTOR_DELAY = 1000; // 1s
static const unsigned int NON_COMMON_CONTACTOR_CHECK_DELAY = 1000; // 1s
static const unsigned int NON_COMMON_RETRY_CONTACTOR_DELAY = 10000; // 10s
