#pragma once
#include "main.h"
#include "CanTxGatekeeperQueueData.h"
#include "cmsis_os.h"
extern const CAN_TxHeaderTypeDef baseCanTxHdr;
extern osMessageQueueId_t canTxGatekeeperQueue;
