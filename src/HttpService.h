#ifndef HttpService_h
#define HttpService_h
#include "Arduino.h"
#include "ArduinoJson.h"
#include <ESP8266HttpClient.h>
#include "DateTimeModel.h"
#include "UrlEncoderDecoderService.h"

class HttpService
{
    private:
        String _cscsBaseUrl;
        String _getDateTimeRoute;
        String _sendTextUrl;

        WiFiClient _wifiClient;
        HTTPClient _client;
        UrlEncoderDecoderService _urlEncoderDecoder;

        void SendErrorMessageAsText();

    public:
        HttpService(String cscsBaseUrl, String getDateTimeRoute, String sendTextUrl);
        DateTimeModel GetDateTime();
        
};

#endif