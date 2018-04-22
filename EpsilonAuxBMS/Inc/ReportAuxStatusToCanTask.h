#pragma once

#include "AuxCan.h"
#include "AuxStatus.h"

// Task for sending aux status via CAN
// arg: osMutexId* canHandleMutex
void reportAuxToCanTask(void const* arg);
