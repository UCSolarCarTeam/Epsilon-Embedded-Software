#include "SetContactorsTask.h"

static const uint32_t AUX_SET_CONTACTOR_FREQ = 1000; // Every second
static const float CURRENT_SENSE_RESISTOR  = 0.001; //actually 1 mOhm, but 1 Ohm for testing
static const int GAIN  = 250;
static const uint32_t SENSE_SETUP_TIME  = 50; // Setup time for current_sense
static const float CURRENT_LOWER_THRESHOLD  = 0.15; // Lower current threshold
static const uint32_t ADC_POLL_TIMEOUT  = 10;
static const int MAX_ATTEMPTS  = 2; // max attempts for trying to turn on a contactor

typedef enum State {FIRST_CHECK, COMMON_CONTACTOR_ON, CHARGE_CONTACTOR_ON, DISCHARGE_CONTACTOR_ON,
            DONE, BLOCKED} State;

void setContactorsTask(void const* arg)
{
    // For concurrency with updateChargeStatusTask()
    osMutexId* auxStatusMutex = (osMutexId*) arg;
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    int attemptCount = 0; // Keep track of how many attempts there were to turn on a contactor
    State state = FIRST_CHECK;
    State prev_state;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        if (osMutexWait(auxStatusMutex, 0) != osOK)
        {
            continue;
        }

        if (attemptCount > MAX_ATTEMPTS) // If it's greater than a certain number of attempts, report to CAN that something is wrong
        {
            auxStatus.contactorError = 1;
        }
        else
        {
            auxStatus.contactorError = 0;
        }

        // Check everuthing is all good from orion's side
        if (!(orionStatus.gpioOk && orionStatus.batteryVoltagesInRange))
        {
            prev_state = state;
            state = BLOCKED;
        }

        switch (state)
        {
            case FIRST_CHECK:

                // Check current is low before enabling (ADC conversion and normalization)

                // If current is high for some reason, increment attempt count and cycle again
                if (checkContactor(0xFF, NULL, 1)) // Don't want to read an actual sense pin
                {
                    attemptCount = 0;
                    // Turn on Common Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_SET);
                    state = COMMON_CONTACTOR_ON;
                }
                else
                {
                    attemptCount++;
                }

                break;

            case COMMON_CONTACTOR_ON:
                if (checkContactor(SENSE1_Pin, SENSE1_GPIO_Port, 1))
                {
                    auxStatus.commonContactorState = 1;
                    attemptCount = 0;
                    // Turn on charge Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_SET);
                    state = CHARGE_CONTACTOR_ON;
                }
                else
                {
                    // If something went wrong when checking contactor, increment attempt count and cycle again
                    attemptCount++;
                    auxStatus.commonContactorState = 0;
                }

                break;

            case CHARGE_CONTACTOR_ON:
                if (checkContactor(SENSE2_Pin, SENSE2_GPIO_Port, 2))
                {
                    attemptCount = 0;
                    auxStatus.chargeContactorState = 1;
                    // Turn on discharge contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET);
                    state = DISCHARGE_CONTACTOR_ON;
                }
                else
                {
                    attemptCount++;
                    auxStatus.chargeContactorState = 0;
                }

                break;

            case DISCHARGE_CONTACTOR_ON:
                if (checkContactor(SENSE3_Pin, SENSE3_GPIO_Port, 3))
                {
                    attemptCount = 0;
                    auxStatus.dischargeContactorState = 1;
                    // Enable high voltage
                    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
                    state = DONE;
                }
                else
                {
                    attemptCount++;
                    auxStatus.dischargeContactorState = 0;
                }

                break;

            case DONE: ; // Put this here because compiler kept complaining about a declaration not being a statement
                         // could potentially cause problems.
                uint8_t common = !HAL_GPIO_ReadPin(SENSE1_GPIO_Port, SENSE1_Pin);
                uint8_t charge = !HAL_GPIO_ReadPin(SENSE2_GPIO_Port, SENSE2_Pin);
                uint8_t discharge = !HAL_GPIO_ReadPin(SENSE3_GPIO_Port, SENSE3_Pin);

                if (!common && !charge && !discharge) // None of the contactors are enabled
                {
                    state = FIRST_CHECK;
                }
                else if (!charge || !discharge) // Charge or discharge aren't enabled
                {
                    if (!charge) // If charge isn't enabled, turn on charge
                    {
                        HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_SET);
                        state = CHARGE_CONTACTOR_ON;
                    }
                    else // If discharge isn't enabled, turn on discharge
                    {
                        HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET);
                        state = DISCHARGE_CONTACTOR_ON;
                    }
                }

                break;

            case BLOCKED:
                if (orionStatus.gpioOk && orionStatus.batteryVoltagesInRange)
                {
                    state = prev_state;
                }

                break;

            default:
                state = FIRST_CHECK;
        }

        osMutexRelease(auxStatusMutex);
    }
}

int checkContactor(uint16_t pin, GPIO_TypeDef* port, int current_multiplier)
{
    float pwr_voltage = 0.0;
    float pwr_current = 0.0;
    uint8_t sense_pin;
    // Get 12bit current analog input
    uint32_t current_sense = readCurrent();

    if (current_sense == 0xFFFFFFFF)
    {
        return 0;
    }

    pwr_voltage = 3.3 * current_sense / 0xFFF / GAIN; // change ADC value into the voltage read
    pwr_current = pwr_voltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

    if (pin == 0xFF && port == NULL)
    {
        sense_pin = 1;
    }
    else
    {
        sense_pin = !HAL_GPIO_ReadPin(port, pin);
    }

    // If current is high for some reason or sense pin is high something is wrong
    if (pwr_current > CURRENT_LOWER_THRESHOLD * current_multiplier || !sense_pin)
    {
        return 0;
    }

    return 1;
}

uint32_t readCurrent(void)
{
    // Enable current amplifier. Allow time to settle and take multiple valid concurrent readings
    // to make sure it isn't changing
    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);
    uint32_t sense = 0;
    uint32_t prev_sense = 0;
    int counter = 0;

    while (counter < 3)
    {
        osDelay(SENSE_SETUP_TIME);

        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            sense = HAL_ADC_GetValue(&hadc1);
            int diff = sense - prev_sense;

            if (diff < 0)
            {
                diff *= -1;
            }

            if (diff > 50)
            {
                prev_sense = sense;
                counter = 0;
            }
            else
            {
                counter++;
            }
        }
        else
        {
            sense = 0xFFFFFFFF;
            break;
        }
    }

    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
    return sense;
}
