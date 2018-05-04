#include <string.h> // for memcpy

#include "cmsis_os.h"

#include "DriverControls.h"

#define REGEN_QUEUE_SIZE 5
#define ACCEL_QUEUE_SIZE 5

static float regenValuesQueue[REGEN_QUEUE_SIZE] = {0};
static float accelValuesQueue[ACCEL_QUEUE_SIZE] = {0};

uint32_t getAvgRegen() {
    float sum = 0;
    for(int i = 0; i < REGEN_QUEUE_SIZE; i++) {
        sum += regenValuesQueue[i];
    }
    return (uint32_t)((sum / (float)REGEN_QUEUE_SIZE));
}

uint32_t getAvgAccel() {
    float sum = 0;
    for(int i = 0; i < ACCEL_QUEUE_SIZE; i++) {
        sum += accelValuesQueue[i];
    }
    return (uint32_t)((sum / (float)ACCEL_QUEUE_SIZE));
}

void sendHeartbeatTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, HEARTBEAT_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);
        /// Populate CAN Message
        msg->StdId = HEARTBEAT_STDID;
        msg->DLC = HEARTBEAT_DLC;
        msg->Data[0] = 1;
        // Send CAN Message
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendLightsTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);
        // Zero CAN Message
        memset(msg->Data, 0, 1);
        // Populate CAN Message
        msg->StdId = LIGHTS_STDID;
        msg->DLC = LIGHTS_DLC;
        msg->Data[0] |= 0x01 * !HAL_GPIO_ReadPin(HEADLIGHTS_OFF_GPIO_Port, HEADLIGHTS_OFF_Pin);
        msg->Data[0] |= 0x02 * !HAL_GPIO_ReadPin(HEADLIGHTS_LOW_GPIO_Port, HEADLIGHTS_LOW_Pin);
        msg->Data[0] |= 0x04 * !HAL_GPIO_ReadPin(HEADLIGHTS_HIGH_GPIO_Port, HEADLIGHTS_HIGH_Pin);
        msg->Data[0] |= 0x08 * !HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin);
        msg->Data[0] |= 0x10 * !HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin);
        msg->Data[0] |= 0x20 * !HAL_GPIO_ReadPin(HAZARDS_GPIO_Port, HAZARDS_Pin);
        msg->Data[0] |= 0x40 * !HAL_GPIO_ReadPin(INTERIOR_GPIO_Port, INTERIOR_Pin);
        // Send CAN Message
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendMusicTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MUSIC_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);
        // Zero CAN Message
        memset(msg->Data, 0, 1);
        // Populate CAN Message
        msg->StdId = MUSIC_STDID;
        msg->DLC = MUSIC_DLC;
        msg->Data[0] |= 0x01 * !HAL_GPIO_ReadPin(VOLUME_UP_GPIO_Port, VOLUME_UP_Pin);
        msg->Data[0] |= 0x02 * !HAL_GPIO_ReadPin(VOLUME_DOWN_GPIO_Port, VOLUME_DOWN_Pin);
        msg->Data[0] |= 0x04 * !HAL_GPIO_ReadPin(NEXT_SONG_GPIO_Port, NEXT_SONG_Pin);
        msg->Data[0] |= 0x08 * !HAL_GPIO_ReadPin(LAST_SONG_GPIO_Port, LAST_SONG_Pin);
        // Send CAN Message
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendDriverTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DRIVER_CAN_FREQ);
        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);
        // Zero CAN Message
        memset(msg->Data, 0, 4);

        // Populate CAN Message
        msg->StdId = DRIVER_STDID;
        msg->DLC = DRIVER_DLC;
        // Populate analog inputs
        msg->Data[0] |= (getAvgAccel() & 0x000000ffUL);
        msg->Data[1] |= (getAvgAccel() & 0x00000f00UL) >> 8; // Use first 4 bits|
        msg->Data[1] |= (getAvgRegen() & 0x0000000fUL) << 4; // Use last 4 bits
        msg->Data[2] |= (getAvgRegen() & 0x00000ff0UL) >> 4;
        // Populate GPIO inputs
        msg->Data[3] |= 0x01 * !HAL_GPIO_ReadPin(BRAKES_GPIO_Port, BRAKES_Pin);
        msg->Data[3] |= 0x02 * !HAL_GPIO_ReadPin(FORWARD_GPIO_Port, FORWARD_Pin);
        msg->Data[3] |= 0x04 * !HAL_GPIO_ReadPin(REVERSE_GPIO_Port, REVERSE_Pin);
        msg->Data[3] |= 0x08 * !HAL_GPIO_ReadPin(PUSH_TO_TALK_GPIO_Port, PUSH_TO_TALK_Pin);
        msg->Data[3] |= 0x10 * !HAL_GPIO_ReadPin(HORN_GPIO_Port, HORN_Pin);
        msg->Data[3] |= 0x20 * !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin);
        msg->Data[3] |= 0x40 * !HAL_GPIO_ReadPin(AUX_GPIO_Port, AUX_Pin);
        //Send CAN Message
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
    }
}

void sendDriveCommandsTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();
    float newRegen = 0;
    float newAccel = 0;
    uint8_t forward = 0;
    uint8_t reverse = 0;
    uint8_t reset = 0;
    float busCurrentOut = 1.0f; // Percentage 0 -1 always 100%
    uint32_t motorVelocityOut = 0; // RPM
    float motorCurrentOut = 0.0f; // Percentage 0 - 1
    uint8_t prevResetStatus = 0;
    float dataToSendFloat[2];

    uint8_t regenQueueIndex = 0;
    uint8_t accelQueueIndex = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DRIVE_COMMANDS_FREQ);

        // Read analog inputs
        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            newRegen = (((float)HAL_ADC_GetValue(&hadc1)) / ((float)MAX_ANALOG)) * 100.0;
        }
        else
        {
            newRegen = 0;
        }

        regenValuesQueue[regenQueueIndex++] = newRegen;


        if (HAL_ADC_PollForConversion(&hadc2, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            newAccel = (((float)HAL_ADC_GetValue(&hadc2)) / ((float)MAX_ANALOG)) * 100.0;
        }
        else
        {
            newAccel = 0;
        }

        accelValuesQueue[accelQueueIndex++] = newAccel;

        accelQueueIndex %= REGEN_QUEUE_SIZE;
        regenQueueIndex %= ACCEL_QUEUE_SIZE;

        uint32_t regenPercentage = ((float)getAvgRegen()) / ((float)MAX_ANALOG);
        uint32_t accelPercentage = ((float)getAvgAccel()) / ((float)MAX_ANALOG);

        // Read GPIO Inputs
        forward = !HAL_GPIO_ReadPin(FORWARD_GPIO_Port, FORWARD_Pin); // `!` for active low
        reverse = !HAL_GPIO_ReadPin(REVERSE_GPIO_Port, REVERSE_Pin);

        // Determine data to send
        if (forward && reverse) // Error state
        {
            motorVelocityOut = 0;
            motorCurrentOut = 0;
        }
        else if (regenPercentage > NON_ZERO_THRESHOLD) // Regen state
        {
            motorVelocityOut = 0;
            motorCurrentOut = regenPercentage * REGEN_INPUT_SCALING;
        }
        else if (forward) // Forward state
        {
            motorVelocityOut = MAX_FORWARD_RPM;
            motorCurrentOut = accelPercentage;
        }
        else if (reverse) // Reverse state
        {
            motorVelocityOut = MAX_REVERSE_RPM;
            motorCurrentOut = accelPercentage;
        }
        else  // Off state
        {
            motorVelocityOut = 0;
            motorCurrentOut = 0;
        }

        // Allocate CAN Message, deallocated by sender "sendCanTask()"
        CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);
        // Transmit Motor Drive command
        msg->StdId = MOTOR_DRIVE_STDID;
        msg->DLC = MOTOR_DRIVE_DLC;
        dataToSendFloat[0] = motorVelocityOut;
        dataToSendFloat[1] = motorCurrentOut;
        memcpy(msg->Data, dataToSendFloat, sizeof(float) * 2);
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
        // Allocate new CAN Message, deallocated by sender "sendCanTask()"
        msg = (CanMsg*)osPoolAlloc(canPool);
        //Transmit Motor Power command
        msg->StdId = MOTOR_POWER_STDID;
        msg->DLC = MOTOR_POWER_DLC;
        dataToSendFloat[0] = 0.0f; // Reserved (defined by WaveSculptor datasheet)
        dataToSendFloat[1] = busCurrentOut;
        memcpy(msg->Data, dataToSendFloat, sizeof(float) * 2);
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
        // Transmit Motor Reset command if button switch went from off to on
        reset = !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin); // `!` for active low

        if (!prevResetStatus && reset) /// off -> on
        {
            // Allocate new CAN Message, deallocated by sender "sendCanTask()"
            msg = (CanMsg*)osPoolAlloc(canPool);
            msg->StdId = MOTOR_RESET_STDID;
            osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
        }

        prevResetStatus = !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin);
    }
}

void sendCanTask(void const* arg)
{
    for (;;)
    {
        osEvent evt = osMessageGet(canQueue, osWaitForever); // Blocks

        if (evt.status == osEventMessage)
        {
            CanMsg* msg = (CanMsg*)evt.value.p;
            // Populate CAN Message
            hcan2.pTxMsg->StdId = msg->StdId;
            hcan2.pTxMsg->DLC = msg->DLC;
            memcpy(hcan2.pTxMsg->Data, msg->Data, sizeof(uint8_t) * msg->DLC);
            // Send CAN Message
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            HAL_CAN_Transmit_IT(&hcan2);
            // Deallocate CAN message
            osPoolFree(canPool, msg);
        }
    }
}

