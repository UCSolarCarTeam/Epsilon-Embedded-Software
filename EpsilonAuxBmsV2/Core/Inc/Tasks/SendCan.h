#pragma once
#include "main.h"
#include "CanTxGatekeeperQueueData.h"
#include "cmsis_os.h"
extern const CAN_TxHeaderTypeDef BASE_CAN_TX_HDR;
extern osMessageQueueId_t canTxGatekeeperQueue;
