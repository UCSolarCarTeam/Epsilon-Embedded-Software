#include "cmsis_os.h"

#include "Lights.h"

void updateLightsTask(void const* arg)
{
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();
    // Store inputs values
    

    updateLights lightsCharacteristics;
    

    // NOTE: All Lights Out pins are active low
    for (;;)
    {
        updateLights1 (&lightsCharacteristics, &prevWakeTime);
    }
}

void updateLights1 (updateLights* lightsCharacteristics, uint32_t* prevWakeTime)
{
        lightsCharacteristics -> regenBrakeInt = 0;
        lightsCharacteristics -> regenBrakeFloat = 0;

        osDelayUntil(prevWakeTime, LIGHTS_UPDATE_FREQ);
        lightsCharacteristics -> headlightsOff = (lightsInputs >> HOFF_INPUT_INDEX) & 1;
        lightsCharacteristics -> headlightsLow = (lightsInputs >> HLOW_INPUT_INDEX) & 1;
        lightsCharacteristics -> headlightsHigh = (lightsInputs >> HHIGH_INPUT_INDEX) & 1;
        lightsCharacteristics -> rightSignal = (lightsInputs >> RSIGNAL_INPUT_INDEX) & 1;
        lightsCharacteristics -> leftSignal = (lightsInputs >> LSIGNAL_INPUT_INDEX) & 1;
        lightsCharacteristics -> hazards = (lightsInputs >> HAZARDS_INPUT_INDEX) & 1;
        lightsCharacteristics -> bmsStrobe = ((auxBmsInputs[1] >> 0) & STROBE_FAULT_MASK) & 1;
        lightsCharacteristics -> regenBrakeInt |= (driversInputs[1] & REGEN_BRAKE_MASK_1) >> 4;
        lightsCharacteristics -> regenBrakeInt |= (driversInputs[2] & REGEN_BRAKE_MASK_2) << 4;

        /* UPDATE HEADLIGHTS */
        if (( lightsCharacteristics -> headlightsOff))
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_OFF);
        }
        else if ((lightsCharacteristics -> headlightsLow && lightsCharacteristics -> headlightsHigh))
        {
            // Error state, turn only the low headlights on.
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, LIGHT_OFF);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(HHIGH_GPIO_Port, HHIGH_Pin, lightsCharacteristics -> headlightsHigh);
            HAL_GPIO_WritePin(HLOW_GPIO_Port, HLOW_Pin, lightsCharacteristics -> headlightsLow);
        }

        /* UPDATE BRAKE LIGHTS */
        lightsCharacteristics -> brakes = (driversInputs[BRAKES_INPUT_INDEX_P1] >> BRAKES_INPUT_INDEX_P2) & 1;
        lightsCharacteristics -> regenBrakeFloat = (float) lightsCharacteristics -> regenBrakeInt;

        if (lightsCharacteristics -> brakes || lightsCharacteristics -> regenBrakeFloat > NON_ZERO_THRESHOLD)
        {
            HAL_GPIO_WritePin(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(BRAKE_GPIO_Port, BRAKE_Pin, LIGHT_OFF);
        }

        /* UPDATE SIGNAL LIGHTS */
        // Set to enable or disable for use in blinkSignalLights
        if (lightsCharacteristics -> hazards)
        {
            sigLightsHandle.left = LIGHT_ON;
            sigLightsHandle.right = LIGHT_ON;
        }
        else
        {
            sigLightsHandle.left = lightsCharacteristics -> leftSignal;
            sigLightsHandle.right = lightsCharacteristics -> rightSignal;
        }

        /* UPDATE BMS STROBE */
        if (lightsCharacteristics -> bmsStrobe)
        {
            HAL_GPIO_WritePin(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
        }
}
// Reasoning for complexity
// Blinkers should turn on immediately if they are turned on from an off state
// Blinkers should turn off immediately if they are turned off
// Blinking frequency should be constant if multiple signal inputs are turned on asynchornously
//      Example: Signal Left ENABLED
//               ~200ms pass
//               Hazards Enabled <- (Left blinker should not reset at this point)
void blinkSignalLights( uint32_t* prevWakeTime, uint32_t* blinkerTimer, uint8_t* prevSigState)
{
    osDelayUntil(prevWakeTime, LIGHTS_UPDATE_FREQ);

        // Check if both signal lights inputs are DISABLED
        if (!sigLightsHandle.left && !sigLightsHandle.right) // Going to DISABLED
        {
            // Reset blinker state and turn blinkers off
            HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, LIGHT_OFF);
            HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, LIGHT_OFF);
            // blinkerTimer will be reset to 0 on ENABLE
            *prevSigState = 0;
        }
        else if (!prevSigState) // Going from DISABLED to ENABLED
        {
            // Prepare to keep blinkers on (If blinkerTimer is within (0 - BLINKER_FREQ), turn blinkers on)
            HAL_GPIO_WritePin(RSIGNAL_GPIO_Port, RSIGNAL_Pin, sigLightsHandle.right);
            HAL_GPIO_WritePin(LSIGNAL_GPIO_Port, LSIGNAL_Pin, sigLightsHandle.left);
            *blinkerTimer = 0;
            *prevSigState = 1;
        }
        else // Going from ENABLED to ENABLED
        {
            // Handle Blinking the signal lights
            if (*blinkerTimer >= BLINKER_FREQ)
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
            if (*blinkerTimer > BLINKER_FREQ * 2)
            {
                // If blinkerTimer is greater than (BLINKER_FREQ*2) reset blinkerTimer to 0
                *blinkerTimer = 0;
            }
            else
            {
                *blinkerTimer += LIGHTS_UPDATE_FREQ;
            }

            // Keep prevSigState = 1
        }
}
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
        blinkSignalLights( &prevWakeTime, &blinkerTimer, &prevSigState);
    }
}

void updateStrobeLight(void const* arg)
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
        updateStrobeLight2(&prevWakeTime, &blinkerTimer, &strobeLight);
    }
}

void updateStrobeLight2(uint32_t* prevWakeTime ,uint32_t* blinkerTimer, char* strobeLight)
{
    osDelayUntil(prevWakeTime, LIGHTS_UPDATE_FREQ);
        *strobeLight = (auxBmsInputs[1] >> 0) & STROBE_FAULT_MASK;

        /*Update BMS Strobe*/
        if (*strobeLight && (*blinkerTimer <= BLINKER_FREQ))
        {
            HAL_GPIO_WritePin(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(ESTROBE_GPIO_Port, ESTROBE_Pin, LIGHT_OFF);
        }

        // Update blinker timer
        if ((*blinkerTimer > BLINKER_FREQ * 2) || !(*strobeLight))
        {
            // If blinkerTimer is greater than (BLINKER_FREQ*2) reset blinkerTimer to 0
            *blinkerTimer = 0;
        }
        else
        {
            *blinkerTimer += LIGHTS_UPDATE_FREQ;
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
        reportLightsToCan(&prevWakeTime, canHandleMutex);
    }
}

void reportLightsToCan(uint32_t* prevWakeTime, osMutexId* canHandleMutex)
{
    osDelayUntil(prevWakeTime, LIGHTS_STATUS_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            return;
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
        hcan2.pTxMsg->Data[0] += stat.lowBeams * 0x01;
        hcan2.pTxMsg->Data[0] += stat.highBeams * 0x02;
        hcan2.pTxMsg->Data[0] += stat.brakes * 0x04;
        hcan2.pTxMsg->Data[0] += stat.leftSignal * 0x08;
        hcan2.pTxMsg->Data[0] += stat.rightSignal * 0x10;
        hcan2.pTxMsg->Data[0] += stat.bmsStrobeLight * 0x20;
        // Send CAN msg
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
}

void sendHeartbeatTask(void const* arg)
{
    // For concurrency with reportLightsToCan()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
       sendHeartbeat(canHandleMutex, &prevWakeTime);
    }
}

void sendHeartbeat(osMutexId* canHandleMutex,uint32_t* prevWakeTime)
{
    osDelayUntil(prevWakeTime, LIGHTS_HEARTBEAT_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            return;
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

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    CanRxMsgTypeDef* msg = hcan->pRxMsg;

    if (msg->StdId == LIGHTS_INPUT_STDID && msg->DLC == 1)
    {
        lightsInputs = msg->Data[0];
    }
    else if (msg->StdId == BATTERY_STAT_ERRORS_STDID && msg->DLC == 5)
    {
        batteryErrors[0] = msg->Data[0];
        batteryErrors[1] = msg->Data[1];
        batteryErrors[2] = msg->Data[2];
        batteryErrors[3] = msg->Data[3];
        batteryErrors[4] = msg->Data[4];
    }
    else if (msg->StdId == DRIVERS_INPUTS_STDID && msg->DLC == 4)
    {
        driversInputs[0] = msg->Data[0];
        driversInputs[1] = msg->Data[1];
        driversInputs[2] = msg->Data[2];
        driversInputs[3] = msg->Data[3];
    }
    else if (msg->StdId == AUXBMS_INPUT_STDID && msg->DLC == 2)
    {
        auxBmsInputs[0] = msg->Data[0];
        auxBmsInputs[1] = msg->Data[1];
    }

    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);

    if (HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
    {
        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    }
}
