#include "cmsis_os.h"

#include "Lights.h"

void updateLightsTask(void const* arg)
{
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();
    // Store inputs values
    char headlightsOff;
    char headlights;
    char rightSignal;
    char leftSignal;
    char hazards;
    char brakes;
    char bmsStrobe;
    uint32_t regenBrakeInt;
    float regenBrakeFloat;

    // NOTE: All Lights Out pins are active low
    for (;;)
    {
        regenBrakeInt = 0;
        regenBrakeFloat = 0;

        osDelayUntil(&prevWakeTime, LIGHTS_UPDATE_FREQ);
        headlightsOff = (lightsInputs >> HOFF_INPUT_INDEX) & 1;
        headlights = (lightsInputs >> HLOW_INPUT_INDEX) & 1;
        rightSignal = (lightsInputs >> RSIGNAL_INPUT_INDEX) & 1;
        leftSignal = (lightsInputs >> LSIGNAL_INPUT_INDEX) & 1;
        hazards = (lightsInputs >> HAZARDS_INPUT_INDEX) & 1;
        bmsStrobe = ((auxBmsInputs[1] >> 0) & STROBE_FAULT_MASK) & 1;
        regenBrakeInt |= (driversInputs[1] & REGEN_BRAKE_MASK_1) >> 4;
        regenBrakeInt |= (driversInputs[2] & REGEN_BRAKE_MASK_2) << 4;

        /* UPDATE HEADLIGHTS */
        if ((headlightsOff))
        {
            HAL_GPIO_WritePin(HEAD_GPIO_Port, HEAD_Pin, LIGHT_OFF);
        }
        else if ((headlights))
        {
            // Error state, turn only the low headlights on.
            HAL_GPIO_WritePin(HEAD_GPIO_Port, HEAD_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(HEAD_GPIO_Port, HEAD_Pin, headlights);
        }

        /* UPDATE BRAKE LIGHTS */
        brakes = (driversInputs[BRAKES_INPUT_INDEX_P1] >> BRAKES_INPUT_INDEX_P2) & 1;
        regenBrakeFloat = (float)regenBrakeInt;

        if (brakes || regenBrakeFloat > NON_ZERO_THRESHOLD)
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
            sigLightsHandle.left = LIGHT_ON;
            sigLightsHandle.right = LIGHT_ON;
        }
        else
        {
            sigLightsHandle.left = leftSignal;
            sigLightsHandle.right = rightSignal;
        }

        /* UPDATE BMS STROBE */
        if (bmsStrobe)
        {
            HAL_GPIO_WritePin(STROBE_GPIO_Port, STROBE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(STROBE_GPIO_Port, STROBE_Pin, LIGHT_OFF);
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
            // Prepare to keep blinkers on (If blinkerTimer is within (0 - BLINKER_FREQ), turn blinkers on)
            HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, sigLightsHandle.right);
            HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, sigLightsHandle.left);
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
                HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, sigLightsHandle.right);
                HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, sigLightsHandle.left);
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

void updatStrobeLight(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();
    // Store inputs values
    // NOTE: All Lights Out pins are active low

    // If blinkerTimer is within (0 - BLINKER_FREQ), turn blinkers on
    // If blinkerTimer is within (BLINKER_FREQ - BLINKER_FREQ*2), keep blinkers off
    // If blinkerTimer is greater than (BLINKER_FREQ*2) reset blinkerTimer to 0
    uint32_t blinkerTimer = 0;
    char strobeLight;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_UPDATE_FREQ);
        strobeLight = (auxBmsInputs[1] >> 0) & STROBE_FAULT_MASK;

        /*Update BMS Strobe*/
        if (strobeLight && (blinkerTimer <= BLINKER_FREQ))
        {
            HAL_GPIO_WritePin(STROBE_GPIO_Port, STROBE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(STROBE_GPIO_Port, STROBE_Pin, LIGHT_OFF);
        }

        // Update blinker timer
        if ((blinkerTimer > BLINKER_FREQ * 2) || !strobeLight)
        {
            // If blinkerTimer is greater than (BLINKER_FREQ*2) reset blinkerTimer to 0
            blinkerTimer = 0;
        }
        else
        {
            blinkerTimer += LIGHTS_UPDATE_FREQ;
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

        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
        {
            osMutexRelease(canHandleMutex);
            continue;
        }


        // Toggle blue LED for every CAN message sent
        HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
        // Set CAN msg address
        canTxHdr.StdId = LIGHTS_STATUS_STDID;
        uint8_t data [1];
        uint32_t mailbox;
        // Initalize to avoid garbage values in [6] anad [7]
        LightsStatus stat = {0};
        // Read lights gpios
        stat.lowBeams = HAL_GPIO_ReadPin(HEAD_GPIO_Port, HEAD_Pin);
        stat.highBeams = 0;
        stat.brakes = HAL_GPIO_ReadPin(BRAKE_GPIO_Port, BRAKE_Pin);
        stat.leftSignal = HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin);
        stat.rightSignal = HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin);
        stat.bmsStrobeLight = HAL_GPIO_ReadPin(STROBE_GPIO_Port, STROBE_Pin);
        data[0] = 0;
        data[0] += stat.lowBeams * 0x01;
        data[0] += stat.highBeams * 0x02;
        data[0] += stat.brakes * 0x04;
        data[0] += stat.leftSignal * 0x08;
        data[0] += stat.rightSignal * 0x10;
        data[0] += stat.bmsStrobeLight * 0x20;
        // Send CAN msg
        HAL_CAN_AddTxMessage(&hcan, &canTxHdr, data, &mailbox);
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

        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
        {
            osMutexRelease(canHandleMutex);
            continue;
        }

        // Toggle green LED for every heartbeat sent
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        // Set CAN msg address
        canTxHdr.StdId = LIGHTS_HEARTBEAT_STDID;
        // Always 1
        uint8_t data [1] = {1};
        uint32_t mailbox;
        // Send CAN msg
        HAL_CAN_AddTxMessage(&hcan, &canTxHdr, data, &mailbox);
        osMutexRelease(canHandleMutex);
    }
}

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    CAN_RxHeaderTypeDef hdr;
    uint8_t data[8] = {0};

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hdr, data) != HAL_OK)
    {
        return;
    }

    if (hdr.StdId == LIGHTS_INPUT_STDID && hdr.DLC == 1)
    {
        lightsInputs = data[0];
    }
    else if (hdr.StdId == BATTERY_STAT_ERRORS_STDID && hdr.DLC == 5)
    {
        batteryErrors[0] = data[0];
        batteryErrors[1] = data[1];
        batteryErrors[2] = data[2];
        batteryErrors[3] = data[3];
        batteryErrors[4] = data[4];
    }
    else if (hdr.StdId == DRIVERS_INPUTS_STDID && hdr.DLC == 4)
    {
        driversInputs[0] = data[0];
        driversInputs[1] = data[1];
        driversInputs[2] = data[2];
        driversInputs[3] = data[3];
    }
    else if (hdr.StdId == AUXBMS_INPUT_STDID && hdr.DLC == 2)
    {
        auxBmsInputs[0] = data[0];
        auxBmsInputs[1] = data[1];
    }


    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}
