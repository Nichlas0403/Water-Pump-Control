#include "FlashService.h"

void FlashService::WriteToFlash(String address, String content)
{
    LittleFS.begin();
    File file = LittleFS.open(address, "w");
    file.print(content);
    file.close();
    LittleFS.end();
}

String FlashService::ReadFromFlash(String address)
{
    LittleFS.begin();
    File file = LittleFS.open(address, "r");
    String content = file.readString();
    file.close();
    LittleFS.end();
    return content;
}

void FlashService::DeleteFromFlash(String address)
{
    LittleFS.begin();
    LittleFS.remove(address);
    LittleFS.end();
}