#include "GPIOService.h"

GPIOService::GPIOService(int relayGPIO)
{
    _relayGPIO = relayGPIO;

    pinMode(_relayGPIO, OUTPUT);
}

void GPIOService::TurnRelayOn()
{
    digitalWrite(_relayGPIO, LOW);
    RelayState = true;
}

void GPIOService::TurnRelayOff()
{
    digitalWrite(_relayGPIO, HIGH);
    RelayState = false;
}