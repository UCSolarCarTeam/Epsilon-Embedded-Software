#pragma once

// For now can only need aux, but will leave the struct for future input additions
typedef struct DriversInput
{
    unsigned aux : 1;
    unsigned forward : 1;
    unsigned reverse : 1;
} DriversInput;

extern DriversInput driversInput;
