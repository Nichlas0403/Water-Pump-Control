#ifndef MathService_h
#define MathService_h
#include "Arduino.h"

class MathService
{
    public:
        unsigned long ConvertMinutesToMillis(byte minutes);
        unsigned long ConvertSecondsToMillis(int seconds);
        unsigned long ConvertHoursToMillis(int hours);
        unsigned long ConvertMinutesToMillis(int minutes);
        double ConvertMillisToHours(unsigned long millis);
        double ConvertMillisToDays(unsigned long millis);
        double ConvertMillisToMinutes(unsigned long millis);
};

#endif