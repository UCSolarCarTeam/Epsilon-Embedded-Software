#include <stdbool.h>

#include "cmsis_os.h"

#include "Lights.h"

void updateLights(void const* arg) {
    LightsRequests* requests = (LightsRequests*) arg;
    for(;;) {
        requests->interior = true;
    }
}

void reportLightsToCan(void const* arg) {
    const uint32_t DELAY_MS = 200;
    uint32_t prevWakeTime = osKernelSysTick();
    for(;;) {
        osDelayUntil(&prevWakeTime, DELAY_MS);

        // REPORT CAN

        prevWakeTime = osKernelSysTick();
    }
}

void setLowBeams(bool set) {

}

bool readLowBeams() {
    return true;
}

void setHighBeams(bool set) {

}

bool readHighBeams() {
    return true;
}

void setBrakeLights(bool set) {

}

bool readBrakeLights() {
    return true;
}

void setLeftSignal(bool set) {

}

bool readLeftSignal() {
    return true;
}

void setRightSignal(bool set) {

}

bool readRightSignal() {
    return true;
}

void setBmsStrobe(bool set) {

}

bool readBmsStrobe() {
    return true;
}
