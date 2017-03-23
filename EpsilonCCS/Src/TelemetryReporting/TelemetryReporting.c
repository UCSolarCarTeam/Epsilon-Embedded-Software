#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "TelemetryReporting.h"

#define CCS_TELEM_FREQ 200 // 5Hz == 200ms

void sendTelemetryTask()
{
    uint32_t prevWakeTime = osKernelSysTick();
    // Savlos defined in https://tinyurl.com/n23nzes
    static unsigned char salvo = 1;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, CCS_TELEM_FREQ);

        sendKeyMotor();
        sendLights();
        sendMotorFaults();
        sendBatteryFaults();
        sendDriverControls();
        sendBattery();

        switch (salvo)
        {
            case 1:
                sendMotorDetails(0);
                sendMppt(0);
                sendMppt(1);
                salvo = 2;
                break;

            case 2:
                sendBattery(0);
                sendMotorDetails(1);
                sendMppt(2);
                salvo = 1;
                break;

            default:
                // Error state
                salvo = 1;
                break;
        }

    }
}

void sendKeyMotor()
{

}

void sendMotorDetails(int n)
{

}

void sendDriverControls()
{

}

void sendMotorFaults()
{

}

void sendBatteryFaults()
{

}

void sendBattery()
{

}

void sendMppt(int n)
{

}

void sendLights()
{

}
