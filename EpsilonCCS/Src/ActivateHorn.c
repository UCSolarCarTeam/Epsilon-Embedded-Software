#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "mxconstants.h"

#include "ActivateHorn.h"
#include "DriverControlData.h"

void activateHornTask(void const* arg)
{
    for (;;)
    {
        if (driverControlData.driverInputs.horn)
        {
            HAL_GPIO_WritePin(HORN_GPIO_Port, HORN_Pin, 1);
        }
        else
        {
            HAL_GPIO_WritePin(HORN_GPIO_Port, HORN_Pin, 0);
        }
    }
}
