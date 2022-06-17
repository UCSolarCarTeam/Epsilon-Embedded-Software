#pragma once

typedef enum
{
    OPEN,
    CLOSING, // Intermediate state between open and closed
    CLOSED,
    CONTACTOR_ERROR
} ContactorState;

typedef struct
{
    ContactorState commonState;
    ContactorState chargeState;
    ContactorState dischargeState;
    unsigned startupDone : 1; // Set when all contactors have been successfully closed for the first time.
    // Only set to 0 on initialization.
    unsigned contactorsDisconnected : 1; // Set when all contactors get disconnected due to an error or when the car is tripping
    unsigned orionHappyForStartup : 1; // TODO: This is to check that the orion messages are good in the very beginning before we check for trips. 
} AuxBmsContactorState;
