#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"

#define HEARTBEAT_CAN_FREQ 1000
#define HEARTBEAT_STDID 0x700U
#define HEARTBEAT_DLC 1

#define LIGHTS_CAN_FREQ 100
#define LIGHTS_STDID 0x701U
#define LIGHTS_DLC 1

#define MUSIC_CAN_FREQ 200
#define MUSIC_STDID 0x702U
#define MUSIC_DLC 1

#define DRIVER_CAN_FREQ 50
#define DRIVER_STDID 0x703U
#define DRIVER_DLC 4

#define DRIVE_COMMANDS_FREQ 100
#define MOTOR_DRIVE_STDID 0x501U
#define MOTOR_DRIVE_DLC 8
#define MOTOR_POWER_STDID 0x502U
#define MOTOR_POWER_DLC 8
#define MOTOR_RESET_STDID 0x503U
#define MOTOR_PERCENTAGE_REDUCER 0.53f

#define ADC_POLL_TIMEOUT 10

#define MAX_FORWARD_RPM 20000
#define MAX_REVERSE_RPM -20000
#define NON_ZERO_THRESHOLD 0.17f
#define MAX_PEDAL_THRESHOLD 0.71f
#define MAX_ANALOG 4095 // 12bit ADC (2^12)
#define REGEN_INPUT_SCALING 0.5f

#define AUXBMS_INPUT_STDID 0x721U

#define LAP_PIN CONTEXT_Pin
#define LAP_GPIO_PORT CONTEXT_GPIO_Port

extern CAN_HandleTypeDef hcan2; // main.c
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern osMessageQId canQueue;
extern osPoolId canPool;
extern uint8_t auxBmsInputs[2];

typedef struct
{
    uint32_t StdId;
    uint32_t DLC;
    uint8_t Data[8];
} CanMsg;

void sendHeartbeatTask(void const* arg);
void sendLightsTask(void const* arg);
void sendMusicTask(void const* arg);
void sendDriverTask(void const* arg);
void sendDriveCommandsTask(void const* arg);
void sendCanTask(void const* arg);
