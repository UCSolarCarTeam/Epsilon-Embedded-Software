#include "SetContactorsTask.h"

/*
  This task allows for the turning on of the common, charge, and discharge contactors.
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

void setContactorsTask(void const* arg)
{
    SetContactorStateInfo stateInfo = {.state = FIRST_CHECK, .prevState = FIRST_CHECK, .hasChargeBeenSet = 0, .hasDischargeBeenSet = 0};

    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_SET_CONTACTOR_FREQ);

        if (safeToSetContactor(stateInfo.state))
        {
            stateInfo.state = BLOCKED;
        }

        switch (stateInfo.state)
        {
            case FIRST_CHECK:
            {
                firstCheck(&stateInfo);
            }

            case COMMON_ENABLE_CHECK:
            {
                commonEnableCheck(&stateInfo);
            }

            case CHARGE_ENABLE_CHECK:
            {
                chargeEnableCheck(&stateInfo);
            }

            case DISCHARGE_ENABLE_CHECK:
            {
                dischargeEnableCheck(&stateInfo);
            }

            case IDLE:
            {
                idle(&stateInfo);
            }

            case BLOCKED:
            {
                blocked(&stateInfo);
            }

            case CONTACTOR_DISCONNECTED:
            case SHUTDOWN:
                break;

            default:
                stateInfo.state = FIRST_CHECK;
        }
    }
}

const uint8_t safeToSetContactor(ContactorsSettingState state)
{
    return (!orionStatus.batteryVoltagesInRange && !auxStatus.startUpSequenceDone && state != CONTACTOR_DISCONNECTED);
}
