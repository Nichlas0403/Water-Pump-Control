#ifndef FlashService_h
#define FlashService_h
#include "Arduino.h"
#include <LittleFS.h>

class FlashService
{
    public:
        void WriteToFlash(String address, String content);
        String ReadFromFlash(String address);
        void DeleteFromFlash(String address);
};

#endif