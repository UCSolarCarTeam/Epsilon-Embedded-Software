#include "cmsis_os.h"

#include "Lights.h"

void updateLightsTask(void const* arg)
{
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();
    // Store inputs values
    char headlightsOff;
    char headlightsLow;
    char headlightsHigh;
    char rightSignal;
    char leftSignal;
    char hazards;
    char brakes;

    // NOTE: All Lights Out pins are active low
    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_UPDATE_FREQ);
        headlightsOff = (lightsInputs >> HOFF_INPUT_INDEX) & 1;
        headlightsLow = (lightsInputs >> HLOW_INPUT_INDEX) & 1;
        headlightsHigh = (lightsInputs >> HHIGH_INPUT_INDEX) & 1;
        rightSignal = (lightsInputs >> RSIGNAL_INPUT_INDEX) & 1;
        leftSignal = (lightsInputs >> LSIGNAL_INPUT_INDEX) & 1;
        hazards = (lightsInputs >> HAZARDS_INPUT_INDEX) & 1;

        /* UPDATE HEADLIGHTS */
        if (headlightsLow)
        {
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);
        }

        if (headlightsHigh)
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
        }

        if (batteryErrors)
        {
            HAL_GPIO_WritePin(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
        }

        if (headlightsLow)
        {
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);
        }

        if (headlightsHigh)
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
        }

        if ((headlightsOff) && (headlightsLow || headlightsHigh))
        {
            // Error state, turn headlights off
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);
        }
        else
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, !headlightsHigh);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, !headlightsLow);
        }

        if (headlightsLow && headlightsHigh)
        {
            // Error state, turn headlights off
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);
        }
        else
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, !headlightsHigh);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, !headlightsLow);
        }

        /* UPDATE BRAKE LIGHTS */
        brakes = (driversInputs[BRAKES_INPUT_INDEX_P1] >> BRAKES_INPUT_INDEX_P2) & 1;

        if (brakes)
        {
            HAL_GPIO_WritePin(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);
        }

        /* UPDATE SIGNAL LIGHTS */
        // Set to enable or disable for use in blinkSignalLights
        if (hazards)
        {
            sigLightsHandle.left = 1;
            sigLightsHandle.right = 1;
        }
        else
        {
            sigLightsHandle.left = leftSignal;
            sigLightsHandle.right = rightSignal;
        }
    }
}

// Reasoning for complexity
// Blinkers should turn on immediately if they are turned on from an off state
// Blinkers should turn off immediately if they are turned off
// Blinking frequency should be constant if multiple signal inputs are turned on asynchornously
//      Example: Signal Left ENABLED
//               ~200ms pass
//               Hazards Enabled <- (Left blinker should not reset at this point)
void blinkSignalLightsTask(void const* arg)
{
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();
    // If blinkerTimer is within (0 - BLINKER_FREQ), turn blinkers on
    // If blinkerTimer is within (BLINKER_FREQ - BLINKER_FREQ*2), keep blinkers off
    // If blinkerTimer is greater than (BLINKER_FREQ*2) reset blinkerTimer to 0
    uint32_t blinkerTimer = 0;
    // If both signal lights disabled, prevSigState = 0 (DISABLED)
    // else prevSigState = 1 (ENABLED)
    uint8_t prevSigState = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_UPDATE_FREQ);

        // Check if both signal lights inputs are DISABLED
        if (!sigLightsHandle.left && !sigLightsHandle.right) // Going to DISABLED
        {
            // Reset blinker state and turn blinkers off
            HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
            HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
            // blinkerTimer will be reset to 0 on ENABLE
            prevSigState = 0;
        }
        else if (!prevSigState) // Going from DISABLED to ENABLED
        {
            HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_ON);
            HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_ON);
            // Prepare to keep blinkers on (If blinkerTimer is within (0 - BLINKER_FREQ), turn blinkers on)
            blinkerTimer = 0;
            prevSigState = 1;
        }
        else // Going from ENABLED to ENABLED
        {
            // Handle Blinking the signal lights
            if (blinkerTimer >= BLINKER_FREQ)
            {
                // If blinkerTimer is within (BLINKER_FREQ - BLINKER_FREQ*2), keep blinkers off
                HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
                HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
            }
            else
            {
                // If blinkerTimer is within (0 - BLINKER_FREQ), turn blinkers on
                HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, !sigLightsHandle.right);
                HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, !sigLightsHandle.left);
            }

            // Update blinker timer
            if (blinkerTimer > BLINKER_FREQ * 2)
            {
                // If blinkerTimer is greater than (BLINKER_FREQ*2) reset blinkerTimer to 0
                blinkerTimer = 0;
            }
            else
            {
                blinkerTimer += LIGHTS_UPDATE_FREQ;
            }

            // Keep prevSigState = 1
        }
    }
}

void reportLightsToCanTask(void const* arg)
{
    // For concurrency with sendHeartbeat()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_STATUS_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        // Toggle blue LED for every CAN message sent
        HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
        // Set CAN msg address
        hcan2.pTxMsg->StdId = LIGHTS_STATUS_STDID;
        // Initalize to avoid garbage values in [6] anad [7]
        LightsStatus stat = {0};
        // Read lights gpios
        stat.lowBeams = HAL_GPIO_ReadPin(HLOW_GPIO_Port, HLOW_Pin);
        stat.highBeams = HAL_GPIO_ReadPin(HHIGH_GPIO_Port, HHIGH_Pin);
        stat.brakes = HAL_GPIO_ReadPin(BRAKE_GPIO_Port, BRAKE_Pin);
        stat.leftSignal = HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin);
        stat.rightSignal = HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin);
        stat.bmsStrobeLight = HAL_GPIO_ReadPin(ESTROBE_GPIO_Port, ESTROBE_Pin);
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[0] += !stat.lowBeams * 0x01;
        hcan2.pTxMsg->Data[0] += !stat.highBeams * 0x02;
        hcan2.pTxMsg->Data[0] += !stat.brakes * 0x04;
        hcan2.pTxMsg->Data[0] += !stat.rightSignal * 0x08;
        hcan2.pTxMsg->Data[0] += !stat.leftSignal * 0x10;
        hcan2.pTxMsg->Data[0] += !stat.bmsStrobeLight * 0x20;
        // Send CAN msg
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendHeartbeatTask(void const* arg)
{
    // For concurrency with reportLightsToCan()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_HEARTBEAT_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        // Toggle green LED for every heartbeat sent
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        // Set CAN msg address
        hcan2.pTxMsg->StdId = LIGHTS_HEARTBEAT_STDID;
        // Always 1
        hcan2.pTxMsg->Data[0] = 1;
        // Send CAN msg
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    CanRxMsgTypeDef* msg = hcan->pRxMsg;
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);

    if (msg->StdId == 0x702U) //LIGHTS_INPUT_STDID
    {
        lightsInputs = msg->Data[0];
    }

    if (msg->StdId == 0x701U) //BATTERY_STAT_STDID
    {
        batteryErrors = msg->Data[0];
    }

    else if (msg->StdId == DRIVERS_INPUTS_STDID)
    {
        driversInputs[0] = msg->Data[0];
        driversInputs[1] = msg->Data[1];
        driversInputs[2] = msg->Data[2];
        driversInputs[3] = msg->Data[3];
    }

    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);
    HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}
