#ifndef TimerService_h
#define TimerService_h
#include "Arduino.h"
#include "MathService.h"

class TimerService
{
    private:
        int minMinute;
        int maxMinute;
        int minHour;
        int maxHour;

        MathService _mathService;

        bool ValidateTimeFormat(int hours, int minutes);

    public:
        TimerService();
        int SetTimer(int hours, int minutes);
};

#endif