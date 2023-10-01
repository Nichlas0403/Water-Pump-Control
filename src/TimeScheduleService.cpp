#include "TimeScheduleService.h"


TimeScheduleService::TimeScheduleService() : _flashService(), _mathService()
{
}

unsigned long TimeScheduleService::CalculateTimeUntil(int hourFrom, int hourTo)
{
    if(hourFrom == hourTo)
    {
        return 0;
    }

    if(hourFrom > hourTo)
    {
        hourTo += 24;
    }

    int hoursUntil = _mathService.ConvertHoursToMillis(hourTo - hourFrom);

    return hoursUntil;
}


bool TimeScheduleService::ValidateTimer(int hoursStart, int hoursEnd)
{
    if(hoursStart < 0 ||
       hoursStart > 23 ||
       hoursEnd < 0 ||
       hoursEnd > 23 ||
       hoursStart == hoursEnd)
    {
        return false;
    }

    return true;
}

