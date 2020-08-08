#pragma once
#include "ContactorGatekeeper.h"

extern osThreadId_t commonContactorGatekeeperTaskHandle;

void commonContactorGatekeeperTask(void* arg);
void commonContactorGatekeeper();
void closeCommonContactor();
void openCommonContactor();
