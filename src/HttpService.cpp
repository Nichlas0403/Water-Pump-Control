#include "HttpService.h"

HttpService::HttpService(String cscsBaseUrl, String getDateTimeRoute, String sendTextUrl) : _wifiClient(), _client(), _urlEncoderDecoder()
{
    _cscsBaseUrl = cscsBaseUrl;
    _getDateTimeRoute = getDateTimeRoute;
    _sendTextUrl = sendTextUrl;
}


DateTimeModel HttpService::GetDateTime()
{
    String route = _cscsBaseUrl + _getDateTimeRoute;

    _client.begin(_wifiClient, route);

    int httpResponseCode = _client.GET();

    DateTimeModel dateTime;

    if(httpResponseCode > 0)
    {
        String response = _client.getString();

        dateTime.Day = response.substring(0,2).toInt();
        dateTime.Month = response.substring(3,5).toInt();
        dateTime.Year = response.substring(6,10).toInt();
        dateTime.Hours = response.substring(12,14).toInt();
        dateTime.Minutes = response.substring(15,17).toInt();
        dateTime.Seconds = response.substring(18,20).toInt();
    }
    else
    {
        SendErrorMessageAsText();
    }
    
    _client.end();
    
    return dateTime;

}

void HttpService::SendErrorMessageAsText()
{
    String route = _cscsBaseUrl + _sendTextUrl;
    String errorMessage = "Water Pump Control: Failed to get datetime from CSCS";

    Serial.println(route + _urlEncoderDecoder.urlencode(errorMessage));

    _client.begin(_wifiClient, route + _urlEncoderDecoder.urlencode(errorMessage));
    _client.sendRequest("POST");
    _client.end();
    Serial.println("Connection to " + _cscsBaseUrl + " has ended.");
}
