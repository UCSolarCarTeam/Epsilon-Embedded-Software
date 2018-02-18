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
    float pwr_current = 0.0;
    char done = 0; // An internal enable for being "done"
    // Sense inputs
    char common = 0;
    char charge = 0;
    char discharge = 0;

    int cycleCount = 0; // Keep track of how many attempts there were to turn on a contactor

    // If it's greater than 1, report to CAN that something is wrong
    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        common = !HAL_GPIO_ReadPin(SENSE1_GPIO_Port, SENSE1_Pin);
        charge = !HAL_GPIO_ReadPin(SENSE2_GPIO_Port, SENSE2_Pin);
        discharge = !HAL_GPIO_ReadPin(SENSE3_GPIO_Port, SENSE3_Pin);

        done = common && charge && discharge;

        if (orionOK && batteryVoltagesOK && !done)
        {
            // Check current is low before enabling (ADC conversion and normalization)
            if (cycleCount > 1)
            {
                auxStatus.contactorError = 1;
            }
            else
            {
                auxStatus.contactorError = 0;
            }

            // Check current is low before enabling (ADC conversion and normalization)

            // Get 12bit current analog input
            if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
            {
                current_sense = HAL_ADC_GetValue(&hadc1);
            }
            else
            {
                current_sense = 0;
            }

            current_sense = 3.3 * current_sense / 0xFFF; // change it into the voltage read
            pwr_current = current_sense / CURRENT_SENSE_RESISTOR; // convert voltage to current

            // If current is high for some reason, skip the rest
            if (pwr_current > CURRENT_LOWER_THRESHOLD)
            {
                cycleCount++;
                continue;
            }

            cycleCount = 0; // Reset cycle count
            // Turn on Common Contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_SET);
            // Wait to allow for current peak
            osDelay(CONTACTOR_WAIT_TIME);

            if (!orionOK || !batteryVoltagesOK)
            {
                continue;
            }

            /* Check that the current is below the threshold and that the sense pin is low*/
            // Do ADC conversion, normalize, and check
            // Enable current sensing
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);
            if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
            {
                current_sense = HAL_ADC_GetValue(&hadc1);
            }
            else
            {
                current_sense = 0;
            }
            // Disable current sensing
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
            current_sense = 3.3 * current_sense / 0xFFF; // change it into the voltage read
            pwr_current = current_sense / CURRENT_SENSE_RESISTOR; // convert voltage to current

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
            // Wait to allow for current peak
            osDelay(CONTACTOR_WAIT_TIME);

            if (!orionOK || !batteryVoltagesOK)
            {
                continue;
            }

            /* Check that the current is below the threshold and that the sense pin is low*/

            // Do ADC conversion, normalize, and check

            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);
            if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
            {
                current_sense = HAL_ADC_GetValue(&hadc1);
            }
            else
            {
                current_sense = 0;
            }
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);

            current_sense = 3.3 * current_sense / 0xFFF; // change it into the voltage read
            pwr_current = current_sense / CURRENT_SENSE_RESISTOR; // convert voltage to current

            charge = !HAL_GPIO_ReadPin(SENSE2_GPIO_Port, SENSE2_Pin);
            auxStatus.chargeContactorState = charge;

            // If current is high for some reason or sense pin is high, skip the rest
            if (pwr_current > CURRENT_LOWER_THRESHOLD * 2 || !common)
            {
                cycleCount++;
                auxStatus.chargeContactorState = 0;
                continue;
            }

            cycleCount = 0;

            // Turn on Discharge Contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET);
            // Wait to allow for current peak
            osDelay(CONTACTOR_WAIT_TIME);

            if (!orionOK || !batteryVoltagesOK)
            {
                continue;
            }

            /* Check that the current is below the threshold and that the sense pin is low*/

            // Do ADC conversion, normalize, and check
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);
            if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
            {
                current_sense = HAL_ADC_GetValue(&hadc1);
            }
            else
            {
                current_sense = 0;
            }
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
            current_sense = 3.3 * current_sense / 0xFFF; // change it into the voltage read
            pwr_current = current_sense / CURRENT_SENSE_RESISTOR; // convert voltage to current

            discharge = !HAL_GPIO_ReadPin(SENSE3_GPIO_Port, SENSE2_Pin);
            auxStatus.dischargeContactorState = discharge;

            // If current is high for some reason or sense pin is high, skip the rest
            if (pwr_current > CURRENT_LOWER_THRESHOLD * 3 || !common)
            {
                cycleCount++;
                auxStatus.dischargeContactorState = 0;
                continue;
            }

            cycleCount = 0;

            // Enable high voltage
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
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
        HAL_SPI_Receive(&hspi3, rxBuff, size, SPI_TIMEOUT);
        // Set Chip Select to be high
        HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);

        // ADC sends 4 (could be 3, will get second opinion) zeros before sending data,
        //so can ignore top 4 bits of rxBuff[0] and bottom 2 bits of rxBuff[1]
        uint16_t upperBits = (uint16_t)(rxBuff[1] << 6);
        uint16_t lowerBits = (uint16_t)(rxBuff[0] & 0xF6); // Don't care about bottom 2 bits
        voltage =  0x03FF & ((upperBits) | (lowerBits >> 2)); // The top 6 bits should be zeros, but just in case

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
            auxStatus.auxVoltage = ((int)relative_voltage + 1)& 0x1F;
        }
    }
}
