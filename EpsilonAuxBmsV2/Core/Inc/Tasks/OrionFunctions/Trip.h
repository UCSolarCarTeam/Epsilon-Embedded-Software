#pragma once
#include "main.h"
#include "OrionCanInfo.h"
#include "AuxTrip.h"
#include "OrionConstants.h"

void updateAuxTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate);
uint8_t checkDischargeTrip(OrionCanInfo* message);
uint8_t checkChargeTrip(OrionCanInfo* message);
uint8_t checkProtectionTrip(OrionCanInfo* message);
