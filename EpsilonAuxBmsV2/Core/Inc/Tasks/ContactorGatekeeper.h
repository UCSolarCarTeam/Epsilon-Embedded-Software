#pragma once
#include "cmsis_os.h"
#include "CheckCurrent.h"
#include "ContactorState.h"
#include "ContactorEventFlags.h"
#include "main.h"

extern osEventFlagsId_t contactorControlEventBits;
extern AuxBmsContactorState auxBmsContactorState;

static const unsigned int CONTACTOR_DELAY = 1000; // 1s
