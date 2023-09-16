#ifndef GPIOService_h
#define GPIOService_h
#include "Arduino.h"

class GPIOService
{
    private:
        int _relayGPIO;

    public:
        bool RelayState;

        GPIOService(int relayGPIO);

        void TurnRelayOn();
        void TurnRelayOff();

};

#endif