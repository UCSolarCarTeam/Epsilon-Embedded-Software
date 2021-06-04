#include "CheckContactorError.h"
/*
  Returns 1 if the contactor state is CONTACTOR_ERROR or the contactor state does not match the contactor sense.
  Otherwise return 0.
*/
uint8_t isContactorError( uint8_t contactorSense, ContactorState contactorState)
{
    return (contactorState == CONTACTOR_ERROR) || (contactorState == CLOSED && !contactorSense) || (contactorState == OPEN && contactorSense);
}
