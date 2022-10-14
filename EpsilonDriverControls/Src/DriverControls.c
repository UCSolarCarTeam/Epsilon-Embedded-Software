#include <string.h> // for memcpy

#include "cmsis_os.h"

#include "DriverControls.h"

float regenValuesQueue[REGEN_QUEUE_SIZE] = {0};
float accelValuesQueue[ACCEL_QUEUE_SIZE] = {0};

uint32_t getAvgRegen()
{
    float sum = 0;

    for (int i = 0; i < REGEN_QUEUE_SIZE; i++)
    {
        sum += regenValuesQueue[i];
    }

    return (uint32_t)((sum / (float)REGEN_QUEUE_SIZE));
}

uint32_t getAvgAccel()
{
    float sum = 0;

    for (int i = 0; i < ACCEL_QUEUE_SIZE; i++)
    {
        sum += accelValuesQueue[i];
    }

    return (uint32_t)((sum / (float)ACCEL_QUEUE_SIZE));
}

float calculateMotorCurrent(float accelPercentage)
{
    // To avoid a software overcurrent, our motor config
    // is set to have 100 A max current, we scale it so we send 69 A
    // on full pedal press

    if ((accelPercentage - NON_ZERO_THRESHOLD) > 0 )
    {
        return (accelPercentage - NON_ZERO_THRESHOLD)
               / (MAX_PEDAL_THRESHOLD - NON_ZERO_THRESHOLD)
               * MOTOR_PERCENTAGE_REDUCER;
    }
    else
    {
        return 0.0;
    }
}

float lowPassFilter(float presentMotorCurrent, float prevMotorCurrent)
{
    // Essentially a simple IIR low pass filter, which is a system that resists change.
    // The purpose is to smooth the output current so that there aren't any big jumps that could cause the motors to trip.
    // The MOTOR_CURRENT_SMOOTHING_FACTOR determines how resistant the low pass filter is to change
    return prevMotorCurrent + MOTOR_CURRENT_SMOOTHING_FACTOR * (presentMotorCurrent - prevMotorCurrent);
}

float calculateAccelMotorCurrent(float accelPercentage, float prevMotorCurrent)
{
    return lowPassFilter(calculateMotorCurrent(accelPercentage), prevMotorCurrent);
}

float calculateRegenMotorCurrent(float regenPercentage, float prevMotorCurrent)
{
    // Scale presentMotorCurrent by REGEN_INPUT_SCALING because regen uses less current than normal acceleration. Motors will trip if current is greater.
    float presentMotorCurrent = calculateMotorCurrent(regenPercentage) * REGEN_INPUT_SCALING ;
    return lowPassFilter(presentMotorCurrent, prevMotorCurrent);
}

uint8_t vehicleVelocitySafeToGoForward()
{
    return (motor0VehicleVelocityInput >= SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD && motor1VehicleVelocityInput >= SAFE_VEHICLE_VELOCITY_TO_GO_FORWARD);
}

uint8_t vehicleVelocitySafeToGoReverse()
{
    return (motor0VehicleVelocityInput <= SAFE_VEHICLE_VELOCITY_TO_GO_REVERSE && motor1VehicleVelocityInput <= SAFE_VEHICLE_VELOCITY_TO_GO_REVERSE);
}

uint8_t isNewDirectionSafe(uint8_t forward, uint8_t reverse)
{
    if (forward && reverse) // Error state (can't go forward and reverse!)
    {
        return 0;
    }
    else if ((forward && vehicleVelocitySafeToGoForward()) || (reverse && vehicleVelocitySafeToGoReverse()))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void sendHeartbeat(uint32_t* prevWakeTimePtr)
{
    osDelayUntil(prevWakeTimePtr, HEARTBEAT_CAN_FREQ);
    // Allocate CAN Message, deallocated by sender "sendCanTask()"
    CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);
    /// Populate CAN Message
    msg->StdId = HEARTBEAT_STDID;
    msg->DLC = HEARTBEAT_DLC;
    msg->Data[0] = 1;
    // Send CAN Message
    osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
}

void sendHeartbeatTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        sendHeartbeat(&prevWakeTime);
    }
}

void sendLights(uint32_t* prevWakeTimePtr)
{
    osDelayUntil(prevWakeTimePtr, LIGHTS_CAN_FREQ);
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

void sendLightsTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        sendLights(&prevWakeTime);
    }
}

void sendMusic(uint32_t* prevWakeTimePtr)
{
    osDelayUntil(prevWakeTimePtr, MUSIC_CAN_FREQ);
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

void sendMusicTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        sendMusic(&prevWakeTime);
    }
}

void sendDriver(uint32_t* prevWakeTimePtr)
{
    osDelayUntil(prevWakeTimePtr, DRIVER_CAN_FREQ);
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
    msg->Data[3] |= 0x80 * !HAL_GPIO_ReadPin(LAP_GPIO_PORT, LAP_PIN);

    //Send CAN Message
    osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
}

void sendDriverTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        sendDriver(&prevWakeTime);
    }
}

void sendDriveCommands(uint32_t* prevWakeTimePtr,
                       DriveCommandsInfo* driveCommandsInfo,
                       uint32_t* switching)
{
    osDelayUntil(prevWakeTimePtr, DRIVE_COMMANDS_FREQ);

    float newRegen = 0;
    float newAccel = 0;

    // Read analog inputs
    if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
    {
        newRegen = (((float)HAL_ADC_GetValue(&hadc1)) / ((float)MAX_ANALOG)) * 100.0; // Convert to full value for reporting
    }

    regenValuesQueue[driveCommandsInfo->regenQueueIndex++] = newRegen;

    if (HAL_ADC_PollForConversion(&hadc2, ADC_POLL_TIMEOUT) == HAL_OK)
    {
        newAccel = (((float)HAL_ADC_GetValue(&hadc2)) / ((float)MAX_ANALOG)) * 100.0;
    }

    accelValuesQueue[driveCommandsInfo->accelQueueIndex++] = newAccel;

    driveCommandsInfo->accelQueueIndex %= REGEN_QUEUE_SIZE;
    driveCommandsInfo->regenQueueIndex %= ACCEL_QUEUE_SIZE;

    // Convert values back to floating percentages for motors
    float regenPercentage = (float)getAvgRegen() / 100.0;
    float accelPercentage = (float)getAvgAccel() / 100.0;

    // Determine drive commands
    uint8_t forward = !HAL_GPIO_ReadPin(FORWARD_GPIO_Port, FORWARD_Pin); // `!` for active low
    uint8_t reverse = !HAL_GPIO_ReadPin(REVERSE_GPIO_Port, REVERSE_Pin);
    uint8_t brake = !HAL_GPIO_ReadPin(BRAKES_GPIO_Port, BRAKES_Pin);

    // Read AuxBMS messages
    char allowCharge = auxBmsInputs[1] & 0x02;
    char allowDischarge = auxBmsInputs[1] & 0x08;

    // Determine data to send
    float motorVelocityOut; // RPM

    if (!isNewDirectionSafe(forward, reverse)) // If new direction input isn't safe, zero outputs
    {
        motorVelocityOut = 0;
        driveCommandsInfo->motorCurrentOut = 0;
    }
    else if (driveCommandsInfo->resetStatus == SettingReset)
    {
        driveCommandsInfo->motorCurrentOut =
                    calculateRegenMotorCurrent(0, driveCommandsInfo->motorCurrentOut);

            if(driveCommandsInfo->motorCurrentOut < SWITCHING_CURRENT) {
                driveCommandsInfo->resetStatus = Resetting;
                driveCommandsInfo->motorCurrentOut = 0;
            }
    }
    else if (regenPercentage > NON_ZERO_THRESHOLD) // Regen state
    {
        // To stop using regen braking, set motorCurrentOut to desired value and zero motorVelocityOut
        // To stop without regen braking, zero both motorCurrentOut and motorVelocityOut
        // https://tritium.com.au/includes/TRI88.004v4-Users-Manual.pdf - Section 13

        if(driveCommandsInfo->motorState == Accelerating) {
            *switching = 1;
        }

        driveCommandsInfo->motorState = RegenBraking;

        if(*switching) {
            driveCommandsInfo->motorCurrentOut =
                    calculateRegenMotorCurrent(0, driveCommandsInfo->motorCurrentOut);

            if(driveCommandsInfo->motorCurrentOut < SWITCHING_CURRENT) {
                *switching = 0;
                driveCommandsInfo->motorCurrentOut = 0;
            }
            
        } else {
            motorVelocityOut = 0;

            // Alow regen braking based on input from AuxBMS
            if (allowCharge)
            {
                driveCommandsInfo->motorCurrentOut =
                    calculateRegenMotorCurrent(regenPercentage, driveCommandsInfo->motorCurrentOut);
            }
            else
            {
                driveCommandsInfo->motorCurrentOut = 0;
            }
        }
    }
    else if (brake) // Mechanical Brake Pressed
    {
        driveCommandsInfo->motorState = MechanicalBreaking;
        motorVelocityOut = 0;
        driveCommandsInfo->motorCurrentOut = 0;
    }
    else if (accelPercentage > NON_ZERO_THRESHOLD) // Drive state
    {
        if(driveCommandsInfo->motorState == RegenBraking) {
            *switching = 1;
        }

        driveCommandsInfo->motorState = Accelerating;

        if(*switching) {
            driveCommandsInfo->motorCurrentOut =
                    calculateAccelMotorCurrent(0, driveCommandsInfo->motorCurrentOut);

            if(driveCommandsInfo->motorCurrentOut < SWITCHING_CURRENT) {
                *switching = 0;
                driveCommandsInfo->motorCurrentOut = 0;
            }
        
        } else {
            if (forward && allowDischarge) // Forward state
            {
                driveCommandsInfo->motorState = Accelerating;
                HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
                motorVelocityOut = MAX_FORWARD_RPM;
                driveCommandsInfo->motorCurrentOut =
                calculateAccelMotorCurrent(accelPercentage, driveCommandsInfo->motorCurrentOut);
            }
            else if (reverse && allowDischarge) // Reverse State
            {
                driveCommandsInfo->motorState = Accelerating;
                HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
                motorVelocityOut = MAX_REVERSE_RPM;
                driveCommandsInfo->motorCurrentOut =
                calculateAccelMotorCurrent(accelPercentage, driveCommandsInfo->motorCurrentOut);
            }
            else
            {
                driveCommandsInfo->motorState = Off;
                motorVelocityOut = 0;
                driveCommandsInfo->motorCurrentOut = 0;
            }
        }
    }
    else // Off state
    {
        if(driveCommandsInfo->motorState == Accelerating) {
            *switching = 1;
        }

        driveCommandsInfo->motorState = Off;

        if(*switching) {
            driveCommandsInfo->motorCurrentOut =
                    calculateRegenMotorCurrent(0, driveCommandsInfo->motorCurrentOut);

            if(driveCommandsInfo->motorCurrentOut < SWITCHING_CURRENT) {
                *switching = 0;
                driveCommandsInfo->motorCurrentOut = 0;
            }
            
        } else {
        driveCommandsInfo->motorState = Off;
        motorVelocityOut = 0;
        driveCommandsInfo->motorCurrentOut = 0;
        }
    }

    // Reset input velocities to default
    // This is TEMPORARY. Should be a fix for this that does a better job of determining if the velocities that were received
    // are stale values. i.e. motor controllers haven't transmitted a message in a while
    motor0VehicleVelocityInput = 0;
    motor1VehicleVelocityInput = 0;

    // Allocate CAN Message, deallocated by sender "sendCanTask()"
    CanMsg* msg = (CanMsg*)osPoolAlloc(canPool);

    // Transmit Motor Drive command
    float dataToSendFloat[2];
    msg->StdId = MOTOR_DRIVE_STDID;
    msg->DLC = MOTOR_DRIVE_DLC;
    dataToSendFloat[0] = motorVelocityOut;
    dataToSendFloat[1] = driveCommandsInfo->motorCurrentOut;
    memcpy(msg->Data, dataToSendFloat, sizeof(float) * 2);
    osMessagePut(canQueue, (uint32_t)msg, osWaitForever);

    // Allocate new CAN Message, deallocated by sender "sendCanTask()"
    msg = (CanMsg*)osPoolAlloc(canPool);

    //Transmit Motor Power command
    msg->StdId = MOTOR_POWER_STDID;
    msg->DLC = MOTOR_POWER_DLC;
    dataToSendFloat[0] = 0.0f; // Reserved (defined by WaveSculptor datasheet)
    dataToSendFloat[1] = BUS_CURRENT_OUT;
    memcpy(msg->Data, dataToSendFloat, sizeof(float) * 2);
    osMessagePut(canQueue, (uint32_t)msg, osWaitForever);

    // Transmit Motor Reset command if button switch went from off to on
    uint8_t reset = !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin); // `!` for active low

    if (!driveCommandsInfo->prevResetStatus && reset) /// off -> on
    {
        driveCommandsInfo->resetStatus = SettingReset;
    }

    if(driveCommandsInfo->resetStatus == Resetting) {
        //Allocate new CAN Message, deallocated by sender "sendCanTask()"
        msg = (CanMsg*)osPoolAlloc(canPool);
        msg->StdId = MOTOR_RESET_STDID;
        osMessagePut(canQueue, (uint32_t)msg, osWaitForever);
        driveCommandsInfo->resetStatus = NotResetting;
    }

    driveCommandsInfo->prevResetStatus = reset;
}

void sendDriveCommandsTask(void const* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    DriveCommandsInfo driveCommandsInfo =
    {
        .motorCurrentOut = 0.0f,
        .motorState = Off,
        .resetStatus = NotResetting,
        .prevResetStatus = 0,
        .regenQueueIndex = 0,
        .accelQueueIndex = 0,
    };

    uint32_t switching = 0;

    for (;;)
    {
        sendDriveCommands(&prevWakeTime, &driveCommandsInfo, &switching);
    }
}

void sendCan()
{
    osEvent evt = osMessageGet(canQueue, osWaitForever); // Blocks

    if (evt.status == osEventMessage)
    {
        
        CanMsg* msg = (CanMsg*)evt.value.p;
        // Populate CAN header
        CAN_TxHeaderTypeDef hdr = {0};
        hdr.StdId = msg->StdId;
        hdr.DLC = msg->DLC;

        // Send CAN Message
        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) > 0)
        {
            uint32_t mailbox;

            if (HAL_CAN_AddTxMessage(&hcan2, &(hdr), msg->Data, &mailbox) == HAL_OK)
            {
                HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
            }
            else 
            {
                HAL_CAN_Init(&hcan2);
            }
        }

        // Deallocate CAN message
        osPoolFree(canPool, msg);
        
    }
}

void sendCanTask(void const* arg)
{
    for (;;)
    {
        sendCan();
    }
}
