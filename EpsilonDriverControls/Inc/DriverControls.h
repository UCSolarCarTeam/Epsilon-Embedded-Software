#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"

#define REGEN_QUEUE_SIZE 5
#define ACCEL_QUEUE_SIZE 5

#define BUS_CURRENT_OUT 1.0f // Percentage 0 -1 always 100%

#define HEARTBEAT_CAN_FREQ 1000
#define HEARTBEAT_STDID 0x700U
#define HEARTBEAT_DLC 1

#define LIGHTS_CAN_FREQ 100
#define LIGHTS_STDID 0x701U
#define LIGHTS_DLC 1

#define MUSIC_CAN_FREQ 200
#define MUSIC_STDID 0x702U
#define MUSIC_DLC 1

#define DRIVER_CAN_FREQ 25
#define DRIVER_STDID 0x703U
#define DRIVER_DLC 4

#define DRIVE_COMMANDS_FREQ 10
#define MOTOR_DRIVE_STDID 0x501U
#define MOTOR_DRIVE_DLC 8
#define MOTOR_POWER_STDID 0x502U
#define MOTOR_POWER_DLC 8
#define MOTOR_RESET_STDID 0x503U
#define MOTOR_PERCENTAGE_REDUCER 1.0f

#define SWITCHING_CURRENT 0.01f

#define ADC_POLL_TIMEOUT 10

#define MAX_FORWARD_RPM 20000
#define MAX_REVERSE_RPM -20000
#define NON_ZERO_THRESHOLD 0.17f
#define MAX_PEDAL_THRESHOLD 0.71f
#define MAX_ANALOG 4095 // 12bit ADC (2^12)
#define REGEN_INPUT_SCALING 0.69f
#define MOTOR_CURRENT_SMOOTHING_FACTOR 0.20f // Smooth current output to prevent big jumps (0-1)

#define AUXBMS_INPUT_STDID 0x721U

#define M0_VELOCITY_INPUT_STDID 0x403U
#define M1_VELOCITY_INPUT_STDID 0x423U

#define SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD -1.38f // A small negative number to have some room for things like slow turns (metres/s)
#define SAFE_VEHICLE_VELOCITY_TO_GO_REVERSE 1.38f // A small positive number to have some room for things like slow turns (metres/s)

#define LAP_PIN CONTEXT_Pin
#define LAP_GPIO_PORT CONTEXT_GPIO_Port

extern CAN_HandleTypeDef hcan2; // main.c
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern osMessageQId canQueue;
extern osPoolId canPool;
extern uint8_t auxBmsInputs[2];
extern float   motor0VehicleVelocityInput;
extern float   motor1VehicleVelocityInput;

typedef struct
{
    uint32_t StdId;
    uint32_t DLC;
    uint8_t Data[8];
} CanMsg;

enum MotorStates {
    Accelerating,
    RegenBraking,
    MechanicalBreaking,
    Off
};

enum ResetStatus {
    NotResetting,
    SettingReset,
    Resetting
};

typedef struct DriveCommandsInfo
{
    float motorCurrentOut;
    enum MotorStates motorState;
    uint8_t prevResetStatus;
    enum ResetStatus resetStatus;
    uint8_t regenQueueIndex;
    uint8_t accelQueueIndex;
} DriveCommandsInfo;

void sendHeartbeat(uint32_t* prevWakeTimePtr);
void sendHeartbeatTask(void const* arg);

void sendLights(uint32_t* prevWakeTimePtr);
void sendLightsTask(void const* arg);

void sendMusic(uint32_t* prevWakeTimePtr);
void sendMusicTask(void const* arg);

void sendDriver(uint32_t* prevWakeTimePtr);
void sendDriverTask(void const* arg);

void sendDriveCommands(uint32_t* prevWakeTimePtr, DriveCommandsInfo* driveCommandsInfo, uint32_t* switching);
void sendDriveCommandsTask(void const* arg);

void sendCan();
void sendCanTask(void const* arg);
