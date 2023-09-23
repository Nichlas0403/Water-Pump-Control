#include "TimeScheduleService.h"


TimeScheduleService::TimeScheduleService() : _flashService(), _mathService()
{
}

String TimeScheduleService::GetCurrentTimeSchedule()
{
    return "";
}

    //11:20 - 10:07: 22 hours and 47 minutes (ST:ST) 23 h, 47 m -- ERROR
    //11:20 - 03:23: 16 hours and 3 minutes (ST:MI), 16 h, 3 m
    //11:20 - 21:50: 10 hours and 30 minutes (MI:MI) 10 ,30 m
    //11:21 - 15:02: 3 hours and 41 minutes (MI:ST) 4 hours, 41 min -- ERROR
    //11:20 - 11:50: 0 hours and 30 minuts (SAME:MI) 0 hours, 30 min
    //11:50 - 11:20: 23 hours and 30 minutes (SAME:ST) 23 hours ,30 min

void TimeScheduleService::SetTimeSchedule(TimeScheduleModel timeSchedule)
{
    
    // DateTimeModel timeInBetweenSchedule = CalculateTimeInBetweenSchedule(timeSchedule);
    // unsigned int hoursAsMillis = _mathService.ConvertHoursToMillis(timeInBetweenSchedule.Hours);
    // unsigned int minutesAsMillis = _mathService.ConvertMinutesToMillis(timeInBetweenSchedule.Minutes);
    // unsigned long timeInBetweenMillis = hoursAsMillis + minutesAsMillis;

    //Hvis den ikke skal start nu
        //Hvornår skal den så starte --> flash

    //hvornår skal den stoppe --> flash

    //når den engang stopper
        //udregn hvornår den skal starte igen og hvornår den skal stoppe igen

    // Check if it's time to turn on the pump
    // if (currentTime.Hours > timeSchedule.StartTime.Hours ||
    //     (currentTime.Hours == timeSchedule.StartTime.Hours && currentTime.Minutes >= timeSchedule.StartTime.Minutes)) 
    // {
    //     unsigned long pumpEndTimeMillis = millis() + timeInBetweenMillis;
    //     //start pump
    //     //calculate millis it should end (millis() += timeInBetweenMillis)
    // } 
    // else 
    // {
    //     _gpioService->TurnRelayOff();
    //     // Pump should be OFF
    //     // Code to turn off the pump here
    // }

    //get datetime
    //if timeSchedule.Start.H
}

unsigned long TimeScheduleService::CalculateTimeUntil(DateTimeModel currentDateTime, DateTimeModel dateTimeTo)
{
    int hoursOn;
    int minutesOn;

    if (currentDateTime.Hours > dateTimeTo.Hours || 
        (currentDateTime.Hours == dateTimeTo.Hours && currentDateTime.Minutes > dateTimeTo.Minutes)) 
    {
        // Case when the end-time is earlier in the day or the same hour but earlier minutes
        hoursOn = 24 - currentDateTime.Hours + dateTimeTo.Hours;
        minutesOn = dateTimeTo.Minutes - currentDateTime.Minutes;
    } 
    else 
    {
        // Case when the end time is later in the day
        hoursOn = dateTimeTo.Hours - currentDateTime.Hours;
        minutesOn = dateTimeTo.Minutes - currentDateTime.Minutes;
    }

    // Ensure minutes are positive
    if (minutesOn < 0) 
    {
        minutesOn += 60;
        hoursOn--;
    }

    DateTimeModel dateTime;
    dateTime.Minutes = minutesOn;
    dateTime.Hours = hoursOn;

    unsigned int hoursAsMillis = _mathService.ConvertHoursToMillis(dateTime.Hours);
    unsigned int minutesAsMillis = _mathService.ConvertMinutesToMillis(dateTime.Minutes);
    unsigned long timeInBetweenMillis = hoursAsMillis + minutesAsMillis;
    timeInBetweenMillis += millis();

    return timeInBetweenMillis;
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