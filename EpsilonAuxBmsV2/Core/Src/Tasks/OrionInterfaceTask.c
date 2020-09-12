#include "OrionInterfaceTask.h"

void orionInterfaceTask(void* arg)
{
    for(;;){
      orionInterface();
    }
}
void orionInterface(void)
{
  OrionInterfaceQueueData* message = NULL;
  osStatus_t status = osMessageQueueGet(orionInterfaceQueue, message, NULL, 400);

  if(status == osErrorTimeout)
  {
    //Set orion can message recieved recently to 0
  }
  else
  {
    if(message->type == CAN_RX)
    {
      processCanMessage(message->value.orionCanInfo);
    }
    else if (message->type == GPIO_EXTI)
    {
      processGpioMessage (message->value.gpioNum);
    }
  }


}

void processCanMessage(OrionCanInfo canInfo){

}
void processGpioMessage(uint16_t gpioNum){

}
