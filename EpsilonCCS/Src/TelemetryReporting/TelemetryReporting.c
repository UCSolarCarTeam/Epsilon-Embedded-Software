#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "TelemetryReporting.h"

#define CCS_TELEM_PERIOD_MS (200) // 5Hz == 200ms

void sendTelemetryTask()
{
    uint32_t prevWakeTime = osKernelSysTick();
    // Salvos defined in https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit#gid=782574835
    static unsigned char salvo = 1;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, CCS_TELEM_PERIOD_MS);

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
