#include "cmsis_os.h"
#include "AuxBMS.h"

void readOrionEnableTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    // Store input values
    char safety = 0;
    char charge = 0;
    char discharge = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_READ_ORION_ENABLE_FREQ);

        safety = HAL_GPIO_ReadPin(CHARGE_SAFETY_SENSE_GPIO_Port, CHARGE_SAFETY_SENSE_Pin);
        charge = HAL_GPIO_ReadPin(CHARGE_ENABLE_SENSE_GPIO_Port, CHARGE_ENABLE_SENSE_Pin);
        discharge = HAL_GPIO_ReadPin(DISCHARGE_ENABLE_SENSE_GPIO_Port, DISCHARGE_ENABLE_SENSE_Pin);

        // Make sure all orion inputs are high. If they aren't, turn off all contactors and strobe light
        if (safety && charge && discharge)
        {
            setContactorEnable = 1;
            auxStatus.strobeBmsLight = 0;
        }
        else
        {
            setContactorEnable = 0;
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_RESET);
            auxStatus.strobeBmsLight = 1;
        }

        // Will also implement reading orion max and min cell voltages
    }
}

void setAuxContactorTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

  uint32_t current = 0;
  float pwr_current = 0.0;
  char done = 0; // An internal enable for being "done"
    // Sense inputs
  char common = 0;
  char charge = 0;
  char discharge = 0;

    uint32_t current = 0;

    unsigned int cycleCount = 0; // Keep track of how many attempts there were to turn on a contactor

    // If it's greater than 1, report to CAN that something is wrong
    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        common = !HAL_GPIO_ReadPin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin);
        charge = !HAL_GPIO_ReadPin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin);
        discharge = !HAL_GPIO_ReadPin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin);

        done = common && charge && discharge;

        if (setContactorEnable && !done)
        {
            // Check current is low before enabling (ADC conversion and normalization)
      if (cycleCount > 1)
      {
        auxStatus.contactorError = 1;
      }
      // Check current is low before enabling (ADC conversion and normalization)

      // Get 12bit current analog input
      if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
      {
          current = HAL_ADC_GetValue(&hadc1);
      }
      else
      {
          current = 0;
      }

      // Some conversion

      // If current is high for some reason, skip the rest
      if (pwr_current > CURRENT_LOWER_THRESHOLD)
      {
        cycleCount++;
        continue;
      }

      cycleCount = 0; // Reset cycle count
      // Turn on Common Contactor
      HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_SET);
      // Wait 1 sec


      /* Check that the current is below the threshold and that the sense pin is low*/
      // Do ADC conversion, normalize, and check

      if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
      {
          current = HAL_ADC_GetValue(&hadc1);
      }
      else
      {
          current = 0;
      }

      // Some conversion

      common = !HAL_GPIO_ReadPin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin);
      // If current is high for some reason or sense pin is high, skip the rest
      if (pwr_current > CURRENT_LOWER_THRESHOLD || !common)
      {
        cycleCount++;
        continue;
      }

      cycleCount = 0;

            // Turn on Common Contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_SET);
            // Wait 1 sec

      /* Check that the current is below the threshold and that the sense pin is low*/

      // Do ADC conversion, normalize, and check

      if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
      {
          current = HAL_ADC_GetValue(&hadc1);
      }
      else
      {
          current = 0;
      }

      // Some conversion

      charge = !HAL_GPIO_ReadPin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin);
      // If current is high for some reason or sense pin is high, skip the rest
      if (pwr_current > CURRENT_LOWER_THRESHOLD || !common)
      {
        cycleCount++;
        continue;
      }

      cycleCount = 0;

      // Turn on Discharge Contactor
      HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET)
      // Wait 1 sec

      /* Check that the current is below the threshold and that the sense pin is low*/

      // Do ADC conversion, normalize, and check

      if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
      {
          current = HAL_ADC_GetValue(&hadc1);
      }
      else
      {
          current = 0;
      }

      // Some conversion

      discharge = !HAL_GPIO_ReadPin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin);
      // If current is high for some reason or sense pin is high, skip the rest
      if (pwr_current > CURRENT_LOWER_THRESHOLD || !common)
      {
        cycleCount++;
        continue;
      }

      cycleCount = 0;

      // Enable high voltage
      HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
    }
}

void updateAuxVoltageTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();
    // Store voltage value
    uint32_t voltage = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_UPDATE_AUX_VOLTAGE_FREQ);

        // Figure out SPI stuff
    }

}
