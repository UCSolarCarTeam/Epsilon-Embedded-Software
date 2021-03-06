#include <string.h> // for memcpy

#include "cmsis_os.h"

#include "CanParser.h"

#include "AuxBmsCanParser.h"
#include "BmsCanParser.h"
#include "DriverControlsCanParser.h"
#include "LightsCanParser.h"
#include "MotorCanParser.h"
#include "MpptCanParser.h"


#define AUX_BMS_CAN_MIN (0x720)
#define AUX_BMS_CAN_MAX (0x72F)

#define BMS_CAN_MIN (0x300)
#define BMS_CAN_MAX (0x30F)

#define DRIVER_CONTROL_CAN_MIN (0x700)
#define DRIVER_CONTROL_CAN_MAX (0x70F)

#define LIGHTS_CAN_MIN (0x710)
#define LIGHTS_CAN_MAX (0x71F)

#define MOTOR_CAN_MIN (0x400)
#define MOTOR_CAN_MAX (0x503)

#define MPPT_CAN_MIN (0x600)
#define MPPT_CAN_MAX (0x603)

#define LED_ON 1
#define LED_OFF 0

void parseCanTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osEvent evt = osMessageGet(canRxQueueId, osWaitForever); // Blocks

        if (evt.status == osEventMessage)
        {
            CanMsg* msg = (CanMsg*)evt.value.p;
            parseCanMessage(msg->StdId, msg->Data);
            osPoolFree(canRxPool, msg);
        }
    }
}

void parseCanMessage(uint32_t stdId, uint8_t* data)
{

    if (stdId >= AUX_BMS_CAN_MIN && stdId <= AUX_BMS_CAN_MAX)
    {
        parseAuxBmsCanMessage(stdId, data);
    }
    else if (stdId >= BMS_CAN_MIN && stdId <= BMS_CAN_MAX)
    {
        parseBmsCanMessage(stdId, data);
    }
    else if (stdId >= DRIVER_CONTROL_CAN_MIN && stdId <= DRIVER_CONTROL_CAN_MAX)
    {
        parseDriverControlsCanMessage(stdId, data);
    }
    else if (stdId >= LIGHTS_CAN_MIN && stdId <= LIGHTS_CAN_MAX)
    {
        parseLightsCanMessage(stdId, data);
    }
    else if (stdId >= MPPT_CAN_MIN && stdId <= MPPT_CAN_MAX)
    {
        parseMpptCanMessage(stdId, data);
    }
    else if (stdId >= MOTOR_CAN_MIN && stdId <= MOTOR_CAN_MAX)
    {
        parseMotorCanMessage(stdId, data);
    }
}

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    CanMsg* msg = (CanMsg*)osPoolAlloc(canRxPool);

    msg->StdId = hcan->pRxMsg->StdId;

    memcpy(msg->Data, hcan->pRxMsg->Data, 8);
    osMessagePut(canRxQueueId, (uint32_t)msg, 0);

    if (HAL_CAN_Receive_IT(hcan, CAN_FIFO0) == osOK)
    {
        HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    }
    else
    {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, LED_OFF);
    }
}
