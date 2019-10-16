#pragma once

#include "AuxCan.h"
#include "AuxStatus.h"

// Task for sending aux status via CAN
// arg: osMutexId* canHandleMutex
void reportAuxStatusToCanTask(void const* arg);
