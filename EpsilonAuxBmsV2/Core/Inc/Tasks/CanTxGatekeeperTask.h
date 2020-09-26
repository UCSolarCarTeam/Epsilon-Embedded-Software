#pragma once
#include "cmsis_os.h"
#include "SendCan.h"
void canTxGatekeeperTask(void* arg);
void canTxGatekeeper(CanTxGatekeeperQueueData* canTxQueueData);

extern CAN_HandleTypeDef hcan1;
