#include <string.h> // for memcpy

#include "cmsis_os.h"

#include "DriverControls.h"

void sendHeartbeatTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, HEARTBEAT_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = HEARTBEAT_STDID;
        hcan2.pTxMsg->DLC = HEARTBEAT_DLC;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendLightsTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = LIGHTS_STDID;
        hcan2.pTxMsg->DLC = LIGHTS_DLC;
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[0] += 0x01 * !HAL_GPIO_ReadPin(HEADLIGHTS_OFF_GPIO_Port, HEADLIGHTS_OFF_Pin);
        hcan2.pTxMsg->Data[0] += 0x02 * !HAL_GPIO_ReadPin(HEADLIGHTS_LOW_GPIO_Port, HEADLIGHTS_LOW_Pin);
        hcan2.pTxMsg->Data[0] += 0x04 * !HAL_GPIO_ReadPin(HEADLIGHTS_HIGH_GPIO_Port, HEADLIGHTS_HIGH_Pin);
        hcan2.pTxMsg->Data[0] += 0x08 * !HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin);
        hcan2.pTxMsg->Data[0] += 0x10 * !HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin);
        hcan2.pTxMsg->Data[0] += 0x20 * !HAL_GPIO_ReadPin(HAZARDS_GPIO_Port, HAZARDS_Pin);
        hcan2.pTxMsg->Data[0] += 0x40 * !HAL_GPIO_ReadPin(INTERIOR_GPIO_Port, INTERIOR_Pin);
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendMusicTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MUSIC_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = MUSIC_STDID;
        hcan2.pTxMsg->DLC = MUSIC_DLC;
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[0] += 0x01 * !HAL_GPIO_ReadPin(AUX_GPIO_Port, AUX_Pin);
        hcan2.pTxMsg->Data[0] += 0x02 * !HAL_GPIO_ReadPin(VOLUME_UP_GPIO_Port, VOLUME_UP_Pin);
        hcan2.pTxMsg->Data[0] += 0x03 * !HAL_GPIO_ReadPin(VOLUME_DOWN_GPIO_Port, VOLUME_DOWN_Pin);
        hcan2.pTxMsg->Data[0] += 0x04 * !HAL_GPIO_ReadPin(NEXT_SONG_GPIO_Port, NEXT_SONG_Pin);
        hcan2.pTxMsg->Data[0] += 0x10 * !HAL_GPIO_ReadPin(LAST_SONG_GPIO_Port, LAST_SONG_Pin);
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendDriverTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t regen = 0;
    uint32_t accel = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DRIVER_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = DRIVER_STDID;
        hcan2.pTxMsg->DLC = DRIVER_DLC;
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[1] = 0;
        hcan2.pTxMsg->Data[2] = 0;
        hcan2.pTxMsg->Data[3] = 0;

        // Regen 12bit analog input
        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            regen = HAL_ADC_GetValue(&hadc1);
        }
        else
        {
            regen = 0;
        }

        hcan2.pTxMsg->Data[0] += (regen & 0x000000ffUL);
        hcan2.pTxMsg->Data[1] += (regen & 0x00000f00UL) >> 8; // Use first 4 bits

        // Acceleration 12bit analog input
        if (HAL_ADC_PollForConversion(&hadc2, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            accel = HAL_ADC_GetValue(&hadc2);
        }
        else
        {
            accel = 0;
        }

        hcan2.pTxMsg->Data[1] += (accel & 0x0000000fUL) << 4; // Use last 4 bits
        hcan2.pTxMsg->Data[2] += (accel & 0x00000ff0UL) >> 4;
        // GPIO Inputs
        hcan2.pTxMsg->Data[3] += 0x01 * !HAL_GPIO_ReadPin(BRAKES_GPIO_Port, BRAKES_Pin);
        hcan2.pTxMsg->Data[3] += 0x02 * !HAL_GPIO_ReadPin(FORWARD_GPIO_Port, FORWARD_Pin);
        hcan2.pTxMsg->Data[3] += 0x04 * !HAL_GPIO_ReadPin(REVERSE_GPIO_Port, REVERSE_Pin);
        hcan2.pTxMsg->Data[3] += 0x08 * !HAL_GPIO_ReadPin(PUSH_TO_TALK_GPIO_Port, PUSH_TO_TALK_Pin);
        hcan2.pTxMsg->Data[3] += 0x10 * !HAL_GPIO_ReadPin(HORN_GPIO_Port, HORN_Pin);
        hcan2.pTxMsg->Data[3] += 0x20 * !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin);
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendDriveCommandsTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t regenPercentage = 0;
    uint32_t accelPercentage = 0;
    uint8_t forward = 0;
    uint8_t reverse = 0;
    uint8_t reset = 0;
    float busCurrentOut = 1.0f; // Percentage 0 -1 always 100%
    uint32_t motorVelocityOut = 0; // RPM
    float motorCurrentOut = 0.0f; // Percentage 0 - 1
    uint8_t prevResetStatus = 0;
    float dataToSendFloat[2];

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DRIVE_COMMANDS_FREQ);

        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            regenPercentage = ((float)HAL_ADC_GetValue(&hadc1)) / ((float)MAX_ANALOG);
        }
        else
        {
            regenPercentage = 0;
        }

        if (HAL_ADC_PollForConversion(&hadc2, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            accelPercentage = ((float)HAL_ADC_GetValue(&hadc2)) / ((float)MAX_ANALOG);
        }
        else
        {
            accelPercentage = 0;
        }

        forward = !HAL_GPIO_ReadPin(FORWARD_GPIO_Port, FORWARD_Pin); // `!` for active low
        reverse = !HAL_GPIO_ReadPin(REVERSE_GPIO_Port, REVERSE_Pin);
        reset = !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin);

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

        // CRITICAL SECTION START
        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        // Transmit Motor Drive command
        hcan2.pTxMsg->StdId = MOTOR_DRIVE_STDID;
        hcan2.pTxMsg->DLC = MOTOR_DRIVE_DLC;
        dataToSendFloat[0] = motorVelocityOut;
        dataToSendFloat[1] = motorCurrentOut;
        memcpy(hcan2.pTxMsg->Data, dataToSendFloat, sizeof(float) * 2);
        HAL_CAN_Transmit_IT(&hcan2);
        //Transmit Motor Power command
        hcan2.pTxMsg->StdId = MOTOR_POWER_STDID;
        hcan2.pTxMsg->DLC = MOTOR_POWER_DLC;
        dataToSendFloat[0] = 0.0f; // Reserved (defined by WaveSculptor datasheet)
        dataToSendFloat[1] = busCurrentOut;
        memcpy(hcan2.pTxMsg->Data, dataToSendFloat, sizeof(float) * 2);
        HAL_CAN_Transmit_IT(&hcan2);

        // Transmit Motor Reset command if button switch went from off to on
        if (!prevResetStatus && reset) /// off -> on
        {
            hcan2.pTxMsg->StdId = MOTOR_RESET_STDID;
            HAL_CAN_Transmit_IT(&hcan2);
        }

        osMutexRelease(canHandleMutex);
        // CRITICAL SECTION END
        prevResetStatus = !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin);
    }
}
