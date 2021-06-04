#pragma once
#include "AuxStatus.h"

AuxStatus auxStatus;

void clearAuxStatus();

void setNominalAuxStatus();

void assertAuxStatusEqual(AuxStatus expected);