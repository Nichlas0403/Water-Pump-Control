#include "TimeScheduleService.h"


TimeScheduleService::TimeScheduleService(HttpService* httpService) : _flashService()
{
    _httpService = httpService;
}

String TimeScheduleService::GetCurrentTimeSchedule()
{
    return "";
}

void TimeScheduleService::SetTimeSchedule(TimeScheduleModel timeSchedule)
{
    //DateTimeModel currentDateTime = _httpService->GetDateTime();
    


    int minutesOn;

    if(timeSchedule.StartTime.Minutes > timeSchedule.EndTime.Minutes)
    {
        minutesOn = 60 - timeSchedule.StartTime.Minutes;
        minutesOn = minutesOn + timeSchedule.EndTime.Minutes;
    }
    else if (timeSchedule.StartTime.Minutes < timeSchedule.EndTime.Minutes)
    {
        minutesOn = timeSchedule.EndTime.Minutes - timeSchedule.StartTime.Minutes;
    }

    //hours froms start to end:
    int hoursOn;

    //11:20 - 10:07: 22 hours and 47 minutes (ST:ST) 23 h, 47 m -----ERROR-----
    //11:20 - 03:23: 16 hours and 3 minutes (ST:MI), 16 h, 3 m
    //11:20 - 21:50: 10 hours and 30 minutes (MI:MI) 10 ,30 m
    //11:21 - 15:02: 3 hours and 42 minutes (MI:ST) 4 hours, 41 min -----ERROR----
    //11:20 - 11:50: 0 hours and 30 minuts (SAME:MI) 0 hours, 30 min
    //11:50 - 11:20: 23 hours and 30 minutes (SAME:ST) 23 hours ,30 min
    if(timeSchedule.StartTime.Hours > timeSchedule.EndTime.Hours)
    {
        hoursOn = 24 - timeSchedule.StartTime.Hours;
        hoursOn = hoursOn + timeSchedule.EndTime.Hours;
    }
    else if(timeSchedule.StartTime.Hours < timeSchedule.EndTime.Hours)
    {
        if(timeSchedule.StartTime.Minutes > timeSchedule.StartTime.Minutes)
        {
            hoursOn = 0;
        }
        else
        {
            hoursOn = timeSchedule.EndTime.Hours - timeSchedule.StartTime.Hours;
        }
    }
    else if(timeSchedule.StartTime.Hours == timeSchedule.EndTime.Hours)
    {
        if(timeSchedule.StartTime.Minutes < timeSchedule.EndTime.Minutes)
        {
            hoursOn = 0;
        }
        else
        {
            hoursOn = 23;
        }
    }
        



}


bool TimeScheduleService::ValidateTimer(TimeScheduleModel timeSchedule)
{
    if(timeSchedule.StartTime.Hours < 0 ||
        timeSchedule.StartTime.Hours > 23 ||
        timeSchedule.StartTime.Minutes < 0 ||
        timeSchedule.StartTime.Minutes > 59)
        {
            return false;
        }

    if(timeSchedule.EndTime.Hours < 0 ||
        timeSchedule.EndTime.Hours > 23 ||
        timeSchedule.EndTime.Minutes < 0 ||
        timeSchedule.EndTime.Minutes > 59)
        {
            return false;
        }

    if(timeSchedule.StartTime.Hours == timeSchedule.EndTime.Hours &&
        timeSchedule.StartTime.Minutes == timeSchedule.EndTime.Minutes)
        {
            return false;
        }

    return true;
}