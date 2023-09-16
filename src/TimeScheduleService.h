#ifndef TimeScheduleService_h
#define TimeScheduleService_h
#include "Arduino.h"
#include "FlashService.h"

class TimeScheduleService
{
    private:
        FlashService _flashService;

    public:
        bool CurrentTimeSchedule;

        TimeScheduleService();

        String GetCurrentTimeSchedule();
        void UpdateTimeSchedule();

};

#endif