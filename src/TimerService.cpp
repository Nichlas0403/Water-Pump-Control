#include "TimerService.h"
#include "Arduino.h"


TimerService::TimerService() : _mathService()
{
    minMinute = 0;
    maxMinute = 59;
    minHour = 0;
    maxHour = 24;

}

int TimerService::SetTimer(int hours, int minutes)
{
    if(!ValidateTimeFormat(hours, minutes))
        return 0;

    int hoursAsMillis = _mathService.ConvertHoursToMillis(hours);
    int minutesAsMillis = _mathService.ConvertMinutesToMillis(minutes);

    int timerMillis = hoursAsMillis + minutesAsMillis;

    return timerMillis;
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

DateTime TimerService::CalculateTimeRemaining(unsigned long timer)
{
  unsigned long timeRemaining = timer - millis();

  int hours = _mathService.ConvertMillisToHours(timeRemaining); //Casting it as int will round down the number
  
  unsigned long timerWithoutHours = timeRemaining - hours;

  hours %= 24;

  int minutes = _mathService.ConvertMillisToMinutes(timerWithoutHours);

  minutes %= 60;

  DateTime time;

  time.Hours = hours;
  time.Minutes = minutes;

  return time;
}