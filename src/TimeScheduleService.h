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
        HttpService *_httpService;

        DateTimeModel CalculateTimeInBetweenSchedule(TimeScheduleModel timeSchedule);

    public:
        bool CurrentTimeSchedule;

        TimeScheduleService(String timeScheduleStartFlash, String timeScheduleEndFlash, String HttpService* httpService);
        bool ValidateTimer(TimeScheduleModel timeSchedule);
        String GetCurrentTimeSchedule();
        void SetTimeSchedule(TimeScheduleModel timeSchedule);

};

#endif