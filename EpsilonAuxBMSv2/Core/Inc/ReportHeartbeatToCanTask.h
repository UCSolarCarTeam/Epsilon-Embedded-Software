#pragma once

#include "AuxCan.h"

// Task for sending heartbeat via CAN
// arg: osMutexId* canHandleMutex
void reportHeartbeatToCanTask(void const* arg);
