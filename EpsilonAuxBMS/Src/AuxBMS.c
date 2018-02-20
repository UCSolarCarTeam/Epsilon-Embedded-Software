#include "cmsis_os.h"
#include "AuxBMS.h"

void readOrionInputTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_READ_ORION_ENABLE_FREQ);

        if (orionBmsInputs[0] > MAX_VOLTAGE)
        {
            batteryVoltagesOK = 0;
            // Turn off charge contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_RESET);
            auxStatus.chargeContactorState = 0;
            auxStatus.allowCharge = 0;
        }
        else
        {
            batteryVoltagesOK = 1;
            auxStatus.allowCharge = 1;
        }

        if (orionBmsInputs[1] < MIN_VOLTAGE)
        {
            batteryVoltagesOK = 0;
            // Turn off High voltage enable and discharge contactor
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_RESET);
            auxStatus.dischargeContactorState = 0;
        }
        else
        {
            batteryVoltagesOK = 1;
        }
    }
}

void setAuxContactorTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    uint32_t current_sense = 0;
    uint32_t prev_sense = 0;
    float pwr_voltage = 0.0;
    float pwr_current = 0.0;
    // Sense inputs
    char common = 0;
    char charge = 0;
    char discharge = 0;

    int cycleCount = 0; // Keep track of how many attempts there were to turn on a contactor
    int count = 0; // Keeps track of how many good ADC reads we get
    int state = FIRST_CHECK;
    int prev_state;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        if (cycleCount > 1) // If it's greater than 1, report to CAN that something is wrong
        {
            auxStatus.contactorError = 1;
        }
        else
        {
            auxStatus.contactorError = 0;
        }

        switch (state)
        {
            case FIRST_CHECK:
                if (orionOK && batteryVoltagesOK)
                {
                    // Check current is low before enabling (ADC conversion and normalization)
                    // Get 12bit current analog input
                    readCurrent(&count, &current_sense, &prev_sense);
                    prev_sense = 0;

                    pwr_voltage = 3.3 * current_sense / 0xFFF / GAIN; // change ADC value into the voltage read
                    pwr_current = pwr_voltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

                    // If current is high for some reason, skip the rest
                    if (pwr_current > CURRENT_LOWER_THRESHOLD)
                    {
                        cycleCount++;
                        continue;
                    }

                    cycleCount = 0;
                    // Turn on Common Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_SET);
                    state = COMMON_CONTACTOR_ON;
                }
                else
                {
                    prev_state = FIRST_CHECK;
                    state = BLOCKED;
                }

                break;

            case COMMON_CONTACTOR_ON:
                if (orionOK && batteryVoltagesOK)
                {
                    /* Check that the current is below the threshold and that the sense pin is low*/
                    // Do ADC conversion, normalize, and check
                    readCurrent(&count, &current_sense, &prev_sense);
                    prev_sense = 0;

                    pwr_voltage = 3.3 * current_sense / 0xFFF / GAIN; // change it into the voltage read
                    pwr_current = pwr_voltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

                    common = !HAL_GPIO_ReadPin(SENSE1_GPIO_Port, SENSE1_Pin);
                    auxStatus.commonContactorState = common;

                    // If current is high for some reason or sense pin is high, skip the rest
                    if (pwr_current > CURRENT_LOWER_THRESHOLD || !common)
                    {
                        cycleCount++;
                        auxStatus.commonContactorState = 0;
                        continue;
                    }

                    cycleCount = 0;
                    // Turn on charge Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_SET);
                    state = CHARGE_CONTACTOR_ON;
                }
                else
                {
                    prev_state = COMMON_CONTACTOR_ON;
                    state = BLOCKED;
                }

                break;

            case CHARGE_CONTACTOR_ON:
                if (orionOK && batteryVoltagesOK)
                {
                    readCurrent(&count, &current_sense, &prev_sense);
                    prev_sense = 0;

                    pwr_voltage = 3.3 * current_sense / 0xFFF / GAIN; // change it into the voltage read
                    pwr_current = pwr_voltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

                    charge = !HAL_GPIO_ReadPin(SENSE2_GPIO_Port, SENSE2_Pin);
                    auxStatus.chargeContactorState = charge;

                    // If current is high for some reason or sense pin is high, skip the rest
                    if (pwr_current > CURRENT_LOWER_THRESHOLD * 2 || !charge)
                    {
                        cycleCount++;
                        auxStatus.chargeContactorState = 0;
                        continue;
                    }

                    cycleCount = 0;
                    // Turn on Discharge Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET);
                    state = DISCHARGE_CONTACTOR_ON;
                }
                else
                {
                    prev_state = CHARGE_CONTACTOR_ON;
                    state = BLOCKED;
                }

                break;

            case DISCHARGE_CONTACTOR_ON:
                if (orionOK && batteryVoltagesOK)
                {
                    readCurrent(&count, &current_sense, &prev_sense);
                    prev_sense = 0;

                    pwr_voltage = 3.3 * current_sense / 0xFFF / GAIN; // change it into the voltage read
                    pwr_current = pwr_voltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

                    discharge = !HAL_GPIO_ReadPin(SENSE3_GPIO_Port, SENSE3_Pin);
                    auxStatus.dischargeContactorState = discharge;

                    // If current is high for some reason or sense pin is high, skip the rest
                    if (pwr_current > CURRENT_LOWER_THRESHOLD * 3 || !discharge)
                    {
                        cycleCount++;
                        auxStatus.dischargeContactorState = 0;
                        continue;
                    }

                    cycleCount = 0;

                    // Enable high voltage
                    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
                    state = DONE;
                }
                else
                {
                    prev_state = DISCHARGE_CONTACTOR_ON;
                    state = BLOCKED;
                }

                break;

            case DONE:
                common = !HAL_GPIO_ReadPin(SENSE1_GPIO_Port, SENSE1_Pin);
                charge = !HAL_GPIO_ReadPin(SENSE2_GPIO_Port, SENSE2_Pin);
                discharge = !HAL_GPIO_ReadPin(SENSE3_GPIO_Port, SENSE3_Pin);

                if (!common && !charge && !discharge) // None of the contactors are enabled
                {
                    state = FIRST_CHECK;
                }
                else if (!charge || !discharge)
                {
                    if (!charge)
                    {
                        HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_SET);
                    }
                    else
                    {
                        HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET);
                    }

                    state = DISCHARGE_CONTACTOR_ON;
                }

                break;

            case BLOCKED:
                if (orionOK && batteryVoltagesOK)
                {
                    state = prev_state;
                }

                break;

            default:
                state = FIRST_CHECK;
        }
    }
}

void updateAuxVoltageTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();
    // Store voltage value
    uint16_t voltage = 0;
    // Size of data to be received
    uint32_t size = 1;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_UPDATE_AUX_VOLTAGE_FREQ);

        // Data buffer
        uint8_t rxBuff[2] = {0, 0};
        // Set Chip Select to be low
        HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET);

        // Start SCLK and receive input
        if (HAL_SPI_Receive(&hspi3, rxBuff, size, SPI_TIMEOUT) == HAL_OK)
        {
            // ADC sends 4 (could be 3, will get second opinion) zeros before sending data,
            //so can ignore top 4 bits of rxBuff[0] and bottom 2 bits of rxBuff[1]
            uint16_t upperBits = (uint16_t)(rxBuff[1] << 6);
            uint16_t lowerBits = (uint16_t)(rxBuff[0] & 0xF6); // Don't care about bottom 2 bits
            voltage =  0x03FF & ((upperBits) | (lowerBits >> 2)); // The top 6 bits should be zeros, but just in case
        }
        else
        {
            voltage = 0;
        }

        // Set Chip Select to be high
        HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);


        float relative_voltage = AUX_NOMINAL_VOLTAGE * voltage / AUX_ADC_NOMINAL_OUTPUT;

        // Rounding
        float diff = relative_voltage - (int)relative_voltage;

        // If the decimal place is less than 0.5, truncate and round down, else round up
        // and keep bottom 5 bits
        if (diff < 0.5)
        {
            auxStatus.auxVoltage = ((int)relative_voltage) & 0x1F;
        }
        else
        {
            auxStatus.auxVoltage = ((int)relative_voltage + 1) & 0x1F;
        }
    }
}

void readCurrent(int* counter, uint32_t* current_sense, uint32_t* prev_sense)
{
    // Enable current amplifier. Allow time to settle and take multiple valid concurrent readings
    // to make sure it isn't changing
    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);

    while (*counter < 3)
    {
        osDelay(SENSE_SETTLING_TIME);

        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            *current_sense = HAL_ADC_GetValue(&hadc1);
            int diff = *current_sense - *prev_sense;

            if (diff < 0)
            {
                diff *= -1;
            }

            if (diff > 50)
            {
                *prev_sense = *current_sense;
                (*counter) = 0;
            }
            else
            {
                (*counter)++;
            }
        }
        else
        {
            *current_sense = 0;
            break;
        }
    }

    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
}
