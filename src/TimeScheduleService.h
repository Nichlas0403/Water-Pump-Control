#ifndef TimeScheduleService_h
#define TimeScheduleService_h
#include "Arduino.h"
#include "FlashService.h"
#include "TimeScheduleModel.h"
#include "HttpService.h"

class TimeScheduleService
{
    private:
        FlashService _flashService;
        HttpService *_httpService;

    public:
        bool CurrentTimeSchedule;

        TimeScheduleService(HttpService* httpService);
        bool ValidateTimer(TimeScheduleModel timeSchedule);
        String GetCurrentTimeSchedule();
        void SetTimeSchedule(TimeScheduleModel timeSchedule);

};

#endif