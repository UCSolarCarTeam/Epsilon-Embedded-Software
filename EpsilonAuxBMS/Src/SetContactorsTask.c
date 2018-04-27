#include "SetContactorsTask.h"
/*
  This task allows for the turnin gon of the common, charge, and discharge contactors.
  It works by checking if the current through the contactors is high for any reason and keeps
  doing so until the current is low. Once the current is low, it turns the common contactor on
  and then waits for one second. Within that second, it is expected that the current spike through
  the contactors would have come down and the contactor sense line should be low. If this isn't
  the case, it tries again. However, if this is the case, it carries on by turning the charge contactor,
  going through the same process, turning on the discharge contactor, going through the same process, and
  finally turning on the high voltage enable line. It then enters a state of being finished.
  If any of the Orion gpio lines are low, or the battery voltages that Orion are not in the expected range,
  it enters a blocked state and waits for everything to return to normal. This is a just a safety measure because in reality
  the whole system should shut down and there shouldn't be power for the board if anything on Orion's side goes wrong.
*/


// Function for reading current through the contactors.
// The current is read from the current amplifier
uint32_t readCurrentThroughContactors(void);
// Function for checking if a specific contactor has been set
int checkIfContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier);

static const uint32_t AUX_SET_CONTACTOR_FREQ = 1000; // Every 1 second
static const float CURRENT_SENSE_RESISTOR  = 0.001; //actually 1 mOhm, but 1 Ohm for testing
static const int GAIN  = 250;
static const uint32_t SENSE_SETUP_TIME  = 50; // Setup time for current_sense
static const float CURRENT_LOWER_THRESHOLD  = 0.15; // Lower current threshold
static const uint32_t ADC_POLL_TIMEOUT  = 10;

typedef enum ContactorsSettingState {FIRST_CHECK,
                    COMMON_CONTACTOR_ON,
                    CHARGE_CONTACTOR_ON,
                    DISCHARGE_CONTACTOR_ON,
                    DONE,
                    BLOCKED} ContactorsSettingState;

void setContactorsTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    ContactorsSettingState state = FIRST_CHECK;
    ContactorsSettingState prevState;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
        {
            continue;
        }

        // Check everuthing is all good from orion's side
        if (!(orionStatus.gpioOk && orionStatus.batteryVoltagesInRange))
        {
            prevState = state;
            state = BLOCKED;
        }

        switch (state)
        {
            case FIRST_CHECK:
                // Check current is low before beginning to turn on contactors
                // If current is high for some reason, cycle again
                auxStatus.contactorError = checkIfContactorSet(0xFF, NULL, 1); // Don't want to read an actual sense pin
                if (auxStatus.contactorError)
                {
                    // Turn on Common Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE1_GPIO_Port, CONTACTOR_ENABLE1_Pin, GPIO_PIN_SET);
                    state = COMMON_CONTACTOR_ON;
                }
                break;

            case COMMON_CONTACTOR_ON:
                auxStatus.contactorError = checkIfContactorSet(SENSE1_Pin, SENSE1_GPIO_Port, 1);
                if (auxStatus.contactorError)
                {
                    auxStatus.commonContactorState = 1;
                    // Turn on charge Contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_SET);
                    state = CHARGE_CONTACTOR_ON;
                }
                else
                {
                    auxStatus.commonContactorState = 0;
                }
                break;

            case CHARGE_CONTACTOR_ON:
                auxStatus.contactorError = checkIfContactorSet(SENSE2_Pin, SENSE2_GPIO_Port, 2);
                if (auxStatus.contactorError)
                {
                    auxStatus.chargeContactorState = 1;
                    // Turn on discharge contactor
                    HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_SET);
                    state = DISCHARGE_CONTACTOR_ON;
                }
                else
                {
                    auxStatus.chargeContactorState = 0;
                }
                break;

            case DISCHARGE_CONTACTOR_ON:
                auxStatus.contactorError = checkIfContactorSet(SENSE3_Pin, SENSE3_GPIO_Port, 3);
                if (auxStatus.contactorError)
                {
                    auxStatus.dischargeContactorState = 1;
                    // Enable high voltage
                    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
                    state = DONE;
                }
                else
                {
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
                    state = prevState;
                }

                break;

            default:
                state = FIRST_CHECK;
        }

        osMutexRelease(auxStatus.auxStatusMutex);
    }
}

int checkIfContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier)
{
    float pwrVoltage = 0.0;
    float pwrCurrent = 0.0;
    uint8_t sense_pin;
    // Get 12bit current analog input
    uint32_t current_sense = readCurrentThroughContactors();

    if (current_sense == 0xFFFFFFFF)
    {
        return 0;
    }

    pwrVoltage = 3.3 * current_sense / 0xFFF / GAIN; // change ADC value into the voltage read
    pwrCurrent = pwrVoltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

    if (pin == 0xFF && port == NULL)
    {
        sense_pin = 1;
    }
    else
    {
        sense_pin = !HAL_GPIO_ReadPin(port, pin);
    }

    // If current is high for some reason or sense pin is high something is wrong
    if (pwrCurrent > CURRENT_LOWER_THRESHOLD * current_multiplier || !sense_pin)
    {
        return 0;
    }

    return 1;
}

/*
  This function takes multiple reads because the current amplifier has a bit of a setup time
  to make sure that erroneous values aren't read, multiple reads are taken till we have a
  relatively steady value. Dan suggested that 3 good reads in a row is good.
*/
uint32_t readCurrentThroughContactors(void)
{
    // Enable current amplifier. Allow time to settle and take multiple valid concurrent readings
    // to make sure it isn't changing
    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);
    uint32_t sense = 0;
    uint32_t prev_sense = 0;

    for (int counter = 0; counter < 3; counter++)
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
                counter = -1; // Will be incremented to 0 at the end of the loop
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
