#pragma once
#include "main.h"
#include "OrionCanInfo.h"
#include "AuxTrip.h"
#include "OrionConstants.h"

uint8_t checkDischargeTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate);
uint8_t checkChargeTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate);
uint8_t checkProtectionTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate);
