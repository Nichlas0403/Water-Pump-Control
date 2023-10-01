#include "MathService.h"
#include "Arduino.h"

unsigned long MathService::ConvertMinutesToMillis(byte minutes)
{
    return 60000 * minutes;
}

unsigned long MathService::ConvertSecondsToMillis(int seconds)
{
    return 1000 * seconds;
}

unsigned long MathService::ConvertHoursToMillis(int hours)
{
    return hours * 3600000;
}

unsigned long MathService::ConvertMinutesToMillis(int minutes)
{
    return minutes * 60000;
}

double MathService::ConvertMillisToHours(unsigned long millis)
{
    return millis * 2.7777777777778E-7;
}

double MathService::ConvertMillisToDays(unsigned long millis)
{
    return millis / 86400000;
}

double MathService::ConvertMillisToMinutes(unsigned long millis)
{
    return millis / 60000;
}