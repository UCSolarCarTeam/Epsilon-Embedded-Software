#include "OrionInterfaceTask.h"

void orionInterfaceTask(void* arg)
{
    for (;;)
    {
        orionInterface();
    }
}
void orionInterface(void)
{
    OrionCanInfo* message = NULL;
    osStatus_t status = osMessageQueueGet(orionInterfaceQueue, message, NULL, 400);

    // Read Orion charge and discharge enable GPIO

    if (status == osErrorTimeout)
    {
        //Set orion can message recieved recently to 0
    }
    else
    {
        // Determine trip conditions and contactor settings based on Orion CAN
    }

    // Determine trip conditions and contactot settings based on Orion GPIO

    // Set Aux Status and Aux Trip

    // Trigger contactor control

}
