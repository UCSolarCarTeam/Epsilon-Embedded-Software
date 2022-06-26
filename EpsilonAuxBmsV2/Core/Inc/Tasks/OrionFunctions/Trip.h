#pragma once
#include "main.h"
#include "OrionCanInfo.h"
#include "AuxTrip.h"
#include "OrionConstants.h"
#include "ContactorGatekeeper.h"

void updateAuxTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate, uint32_t manualChargeTrip);
uint8_t checkDischargeTrip(AuxTrip auxTrip);
uint8_t checkChargeTrip(AuxTrip auxTrip);
