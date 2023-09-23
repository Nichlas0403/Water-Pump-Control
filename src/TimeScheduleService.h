#ifndef TimeScheduleService_h
#define TimeScheduleService_h
#include "Arduino.h"
#include "FlashService.h"
#include "TimeScheduleModel.h"
#include "HttpService.h"
#include "MathService.h"
#include "GPIOService.h"

class TimeScheduleService
{
    private:
        String _timeScheduleStartFlash;
        String _timeScheduleEndFlash;
        FlashService _flashService;
        MathService _mathService;


    public:
        bool CurrentTimeSchedule;

        TimeScheduleService();
        unsigned long CalculateTimeUntil(DateTimeModel dateTimeFrom, DateTimeModel dateTimeTo);
        bool ValidateTimer(TimeScheduleModel timeSchedule);
        String GetCurrentTimeSchedule();
        void SetTimeSchedule(TimeScheduleModel timeSchedule);

};

#endif