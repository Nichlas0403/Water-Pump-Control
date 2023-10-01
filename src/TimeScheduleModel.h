#ifndef TimeScheduleModel_h
#define TimeScheduleModel_h
#include "Arduino.h"
#include "DateTimeModel.h"

class TimeScheduleModel
{
    public:
        DateTimeModel StartTime;
        DateTimeModel EndTime;
};

#endif