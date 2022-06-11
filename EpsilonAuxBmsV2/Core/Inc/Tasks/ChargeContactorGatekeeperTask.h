#pragma once
#include "ContactorGatekeeper.h"

extern osThreadId_t chargeContactorGatekeeperTaskHandle;

void closeChargeContactor();
void openChargeContactor();
void chargeContactorGatekeeperTask(void* arg);
void chargeContactorGatekeeper();
