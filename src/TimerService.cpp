#include "TimerService.h"
#include "Arduino.h"


TimerService::TimerService() : _mathService()
{
    minMinute = 0;
    maxMinute = 59;
    minHour = 0;
    maxHour = 24;

}

String TimerService::SetTimer(int hours, int minutes)
{
    if(!ValidateTimeFormat(hours, minutes))
        return "error";

    int hoursAsMillis = _mathService.ConvertHoursToMillis(hours);
    int minutesAsMillis = _mathService.ConvertMinutesToMillis(minutes);

    int timerMillis = hoursAsMillis + minutesAsMillis;

    return String(timerMillis);
}

bool TimerService::ValidateTimeFormat(int hours, int minutes)
{
    if(minutes < 0 ||
        minutes > 59 ||
        hours < 0 ||
        hours > 24)
        return false;

    return true;
}