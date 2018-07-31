#include "SetContactorsTask.h"
/*
  This task allows for the turnin on of the common, charge, and discharge contactors.
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

static const uint32_t AUX_SET_CONTACTOR_FREQ = 1000; // Every 1 second
static const float CURRENT_SENSE_RESISTOR  = 0.001; //actually 1 mOhm, but 1 Ohm for testing
static const int GAIN  = 250;
static const uint32_t SENSE_SETUP_TIME  = 50; // Setup time for current_sense
static const float CURRENT_LOWER_THRESHOLD  = 0.25; // Lower current threshold
static const uint32_t ADC_POLL_TIMEOUT  = 10;

typedef enum ContactorsSettingState
{
    FIRST_CHECK,
    COMMON_CONTACTOR_ENABLE_CHECK,
    CHARGE_AND_DISCHARGE_CONTACTOR_ENABLE_CHECK,
    DONE,
    BLOCKED,
    CONTACTOR_DISCONNECTED,
    SHUTDOWN
} ContactorsSettingState;

typedef enum Contactor
{
    COMMON,
    CHARGE,
    DISCHARGE,
    NONE
} Contactor;

// Function for reading current through the contactors.
// The current is read from the current amplifier
uint32_t readCurrentThroughContactors(void);
// Function for checking if a specific contactor has been set
int isContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier);
// Turns off all contactors and adjusts auxstatus if isContactorError is 1
void disconnectContactors(uint8_t isContactorError);

void setContactorsTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    ContactorsSettingState state = FIRST_CHECK;
    ContactorsSettingState prevState = state;
    uint8_t contactorError = 0;
    uint8_t common;
    uint8_t charge;
    uint8_t discharge;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        if (!orionStatus.batteryVoltagesInRange && !auxStatus.startUpSequenceDone)
        {
            prevState = state;
            state = BLOCKED;
        }

        switch (state)
        {
            case FIRST_CHECK:
                // Check current is low before beginning to turn on contactors
                // If current is high for some reason, cycle again
                contactorError = !isContactorSet(0xFF, NULL, 1); // Don't want to read an actual sense pin

                if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
                {
                    continue;
                }

                auxStatus.contactorError = contactorError;

                osMutexRelease(auxStatus.auxStatusMutex);

                if (!contactorError)
                {
                    // Turn on Common Contactor
                    HAL_GPIO_WritePin(COMMON_CONTACTOR_ENABLE_GPIO_Port, COMMON_CONTACTOR_ENABLE_Pin, GPIO_PIN_SET);
                    state = COMMON_CONTACTOR_ENABLE_CHECK;
                }

                break;

            case COMMON_CONTACTOR_ENABLE_CHECK:
                contactorError = !isContactorSet(COMMON_SENSE_Pin, COMMON_SENSE_GPIO_Port, 1);

                if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
                {
                    continue;
                }

                auxStatus.contactorError = contactorError;

                osMutexRelease(auxStatus.auxStatusMutex);

                if (!contactorError)
                {
                    state = CHARGE_AND_DISCHARGE_CONTACTOR_ENABLE_CHECK;
                    charge = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
                    discharge = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

                    // Turn on charge and discharge contactors with a delay between them if neither of them
                    // were on. There is no delay if one of them was on. This is done so there isn't any wasted
                    // time
                    if (!charge)
                    {
                        HAL_GPIO_WritePin(CHARGE_CONTACTOR_ENABLE_GPIO_Port, CHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_SET);
                    }

                    if (!charge && !discharge)
                    {
                        osDelay(AUX_SET_CONTACTOR_FREQ);
                        HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_SET);
                    }
                    else if (!discharge)
                    {
                        HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_SET);
                    }
                }

                break;

            case CHARGE_AND_DISCHARGE_CONTACTOR_ENABLE_CHECK:
                contactorError = !isContactorSet(CHARGE_SENSE_Pin, CHARGE_SENSE_GPIO_Port, 3) ||
                                 !isContactorSet(DISCHARGE_SENSE_Pin, DISCHARGE_SENSE_GPIO_Port, 3);

                if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
                {
                    continue;
                }

                auxStatus.contactorError = contactorError;

                if (!auxStatus.startUpSequenceDone)
                {
                    auxStatus.startUpSequenceDone = !contactorError;
                }

                osMutexRelease(auxStatus.auxStatusMutex);

                if (auxStatus.startUpSequenceDone)
                {
                    // Enable high voltage
                    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
                    state = DONE;
                }

                common = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);

                if (!common) // Common contactor has been disconnected
                {
                    disconnectContactors(1);
                    state = CONTACTOR_DISCONNECTED;
                    continue;
                }

                break;

            case DONE:
            {
                common = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
                charge = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
                discharge = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

                uint8_t isChargeTurningOn = 0;
                uint8_t isDischargeTurningOn = 0;

                if (orionStatus.shutOff)
                {
                    state = SHUTDOWN;
                    continue;
                }
                else if (!common)
                {
                    disconnectContactors(1);
                    state = CONTACTOR_DISCONNECTED;
                }

                if (orionStatus.allowCharge && orionStatus.chargeContactorOverriden && !charge)
                {
                    if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
                    {
                        continue;
                    }

                    orionStatus.chargeContactorOverriden = 0;
                    osMutexRelease(orionStatus.orionStatusMutex);

                    isChargeTurningOn = 1;
                    // Turn on charge Contactor and go back to recheck
                    HAL_GPIO_WritePin(CHARGE_CONTACTOR_ENABLE_GPIO_Port, CHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_SET);
                    state = CHARGE_AND_DISCHARGE_CONTACTOR_ENABLE_CHECK;
                }
                else if (orionStatus.allowDischarge && orionStatus.dischargeContactorOverriden && !discharge)
                {
                    if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
                    {
                        continue;
                    }

                    orionStatus.dischargeContactorOverriden = 0;
                    osMutexRelease(orionStatus.orionStatusMutex);

                    isDischargeTurningOn = 1;
                    // Turn on discharge contactor and go back to recheck
                    HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_SET);
                    state = CHARGE_AND_DISCHARGE_CONTACTOR_ENABLE_CHECK;
                }

                if ((orionStatus.allowCharge && !charge && !isChargeTurningOn) ||
                        (orionStatus.allowDischarge && !discharge && !isDischargeTurningOn))
                {
                    disconnectContactors(1);
                    state = CONTACTOR_DISCONNECTED;
                }

                break;
            }

            case BLOCKED:
                common = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
                charge = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
                discharge = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

                if (prevState == CHARGE_AND_DISCHARGE_CONTACTOR_ENABLE_CHECK &&
                        (!common || (auxStatus.chargeContactorState && !charge) ||
                         (auxStatus.dischargeContactorState && !discharge)))
                {
                    disconnectContactors(1);
                    state = CONTACTOR_DISCONNECTED;
                }
                else if (orionStatus.batteryVoltagesInRange)
                {
                    if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
                    {
                        continue;
                    }

                    orionStatus.chargeContactorOverriden = 0;
                    orionStatus.dischargeContactorOverriden = 0;

                    osMutexRelease(orionStatus.orionStatusMutex);
                    state = FIRST_CHECK;
                }

                break;

            case CONTACTOR_DISCONNECTED:
            case SHUTDOWN:
                break;

            default:
                state = FIRST_CHECK;
        }
    }
}

int isContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier)
{
    float pwrVoltage = 0.0;
    float pwrCurrent = 0.0;
    uint8_t sense_pin;
    // Get 12bit current analog input
    uint32_t current_sense = readCurrentThroughContactors();

    if (current_sense == 0xDEADBEEF)
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
                // sense is still not stable
                counter = -1; // Will be incremented to 0 at the end of the loop
            }

            prev_sense = sense;
        }
        else
        {
            sense = 0xDEADBEEF;
            break;
        }
    }

    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
    return sense;
}

void disconnectContactors(uint8_t isContactorError)
{
    // Turn all contactors and high voltage enable off
    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CHARGE_CONTACTOR_ENABLE_GPIO_Port, CHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COMMON_CONTACTOR_ENABLE_GPIO_Port, COMMON_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);

    if (isContactorError)
    {
        while (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK) // Not sure if this is the best idea
            // but will guarantee that we return to this spot
        {
            osDelay(10);
        }

        auxStatus.contactorError = 1;
        auxStatus.strobeBmsLight = 1;
        auxStatus.allowCharge = 0;

        osMutexRelease(auxStatus.auxStatusMutex);
    }
}
