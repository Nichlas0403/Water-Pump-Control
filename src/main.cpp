#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "TimerService.h"
#include "GPIOService.h"
#include "HttpService.h"
#include "FlashService.h"
#include "MathService.h"
#include "TimeScheduleService.h"
#include "DateTimeModel.h"
#include "TimeScheduleModel.h"

#define relayGPIO D7

//Core variables
String _wifiName = "";
String _wifiPassword = "";
unsigned long _timer;
bool _timerOn;
unsigned long _timeScheduleStart;
unsigned long _timeScheduleEnd;
bool _timeScheduleOn;
TimeScheduleModel _timeSchedule;

//FlashService Keys
String _wifiNameFlash = "wifiNameFlash";
String _wifiPasswordFlash = "wifiPassword";
String _cscsBaseUrlFlash = "cscsBaseUrl";
String _getDateTimeRouteFlash = "getDateTimeUrl";
String _sendTextRouteFlash = "sendTextRoute";
String _timeScheduleFlash = "timeSchedule";

//Services
ESP8266WebServer _server(80);
GPIOService _gpioService(relayGPIO);
FlashService _flashService;
TimerService _timerService;
MathService _mathService;
HttpService _httpService(_flashService.ReadFromFlash(_cscsBaseUrlFlash), _flashService.ReadFromFlash(_getDateTimeRouteFlash), _flashService.ReadFromFlash(_sendTextRouteFlash));
TimeScheduleService _timeScheduleService; 


//Function Definitions
void connectToWiFi();
void UpdateTimeScheduleStartAndEnd();
void GetTimeScheduleFromFlash();

void setup() 
{
  Serial.begin(9600);
  _gpioService.TurnRelayOff();

  String timeSchedule = _flashService.ReadFromFlash(_timeScheduleFlash);

  if(timeSchedule != "")
  {
    GetTimeScheduleFromFlash();
    _timeScheduleOn = true;
  }

  _wifiName = _flashService.ReadFromFlash(_wifiNameFlash);
  _wifiPassword = _flashService.ReadFromFlash(_wifiPasswordFlash);

  connectToWiFi();
}

void loop() 
{

  unsigned long currentTime = millis();

  _server.handleClient();
  delay(1);

  if(_timerOn && currentTime > _timer)
  {
    _gpioService.TurnRelayOff();
    _timer = 0;
    _timerOn = false;
  }

  if(_timeScheduleOn && !_gpioService.RelayState && currentTime >= _timeScheduleStart)
  {
    _gpioService.TurnRelayOn();
  }
  else if(_timeScheduleOn && _gpioService.RelayState && currentTime >= _timeScheduleEnd)
  {
    _gpioService.TurnRelayOff();

    UpdateTimeScheduleStartAndEnd();
  }
}

void UpdateTimeScheduleStartAndEnd()
{
    DateTimeModel currentDateTime = _httpService.GetDateTime();

    _timeScheduleStart = _timeScheduleService.CalculateTimeUntil(_timeSchedule.StartTime, currentDateTime);
    _timeScheduleEnd = _timeScheduleService.CalculateTimeUntil(_timeSchedule.EndTime, currentDateTime);
}

void GetTimeScheduleFromFlash()
{
    DynamicJsonDocument timeScheduleJson(1024);
  
    deserializeJson(timeScheduleJson, _flashService.ReadFromFlash(_timeScheduleFlash));

    _timeSchedule.StartTime.Hours = timeScheduleJson["hoursStart"];
    _timeSchedule.StartTime.Minutes = timeScheduleJson["minutesStart"];
    _timeSchedule.EndTime.Hours = timeScheduleJson["hoursEnd"];
    _timeSchedule.EndTime.Minutes = timeScheduleJson["minutesEnd"];
}























// ------------------- API -----------------------  

void HealthCheck()
{
  _server.send(200);
}

void GetState()
{

  String response = "";

  if(_gpioService.RelayState)
    response = "on";
  else
    response = "off";

  _server.send(200, "text/json", response);

}

void GetTimeSchedule()
{


}

void GetTimeRemaining()
{

  if(_timer == 0)
  {
    _server.send(200, "text/json", "TimeRemaining: 00:00");
  }

  DateTimeModel timeRemaining = _timerService.CalculateTimeRemaining(_timer);

  String hoursFormatted;
  
  if(timeRemaining.Hours < 10)
    hoursFormatted = "0" + String(timeRemaining.Hours);
  else
    hoursFormatted = String(timeRemaining.Hours);

  String minutesFormatted;

  if(timeRemaining.Minutes < 10)
    minutesFormatted = "0" + String(timeRemaining.Minutes);
  else
    minutesFormatted = String(timeRemaining.Minutes);
  
  _server.send(200, "text/json", "TimeRemaining: " + hoursFormatted + ":" + minutesFormatted);

}

void TurnWaterPumpOn()
{

  _gpioService.TurnRelayOn();

  _server.send(200);
}

void TurnWaterPumpOff()
{
  _gpioService.TurnRelayOff();

  if(_timerOn)
  {
    _timerOn = false;
  }

  _server.send(200);
}

void SetTimer()
{
  DynamicJsonDocument request(1024);
  
  deserializeJson(request, _server.arg("plain"));

  int minutes = request["minutes"];
  int hours = request["hours"];
  
  _timer = _timerService.SetTimer(hours, minutes);

  if(_timer == 0)
  {
    _server.send(400, "text/json", "Invalid time - minutes must be between 0-59 and hours must be between 0-24");
    return;
  }

  _timer += millis();
  _timerOn = true;

  _gpioService.TurnRelayOn();

  _server.send(200);
}

void UpdateTimeSchedule()
{
  DynamicJsonDocument request(1024);
  
  deserializeJson(request, _server.arg("plain"));

  TimeScheduleModel timeSchedule;

  timeSchedule.StartTime.Hours = request["hoursStart"];
  timeSchedule.StartTime.Minutes = request["minutesStart"];
  timeSchedule.EndTime.Hours = request["hoursEnd"];
  timeSchedule.EndTime.Minutes = request["minutesEnd"];

  bool timerScheduleIsValid = _timeScheduleService.ValidateTimer(timeSchedule);

  if(!timerScheduleIsValid)
  {
    _server.send(400, "text/json", "Invalid time - Hours must be between 0 and 23, minutes must be between 0 and 59, and start and end time can not have the same value.");
    return;
  }

  _flashService.WriteToFlash(_timeScheduleFlash, _server.arg("plain"));
  _timeSchedule = timeSchedule;
  _timeScheduleOn = true;

  DateTimeModel currentDateTime = _httpService.GetDateTime();

  _timeScheduleStart = _timeScheduleService.CalculateTimeUntil(currentDateTime, timeSchedule.StartTime);
  _timeScheduleEnd = _timeScheduleService.CalculateTimeUntil(currentDateTime, timeSchedule.EndTime);
  

  // if(_timerOn)
  // {
  //   _timerOn = false;
  //   _gpioService.TurnRelayOff();
  // }
  
  _server.send(200);

}

// Core server functionality
void restServerRouting() 
{
  _server.on(F("/health-check"), HTTP_GET, HealthCheck);
  _server.on(F("/water-pump/state"), HTTP_GET, GetState);
  _server.on(F("/water-pump/on"), HTTP_PUT, TurnWaterPumpOn);
  _server.on(F("/water-pump/off"), HTTP_PUT, TurnWaterPumpOff);


  _server.on(F("/timer"), HTTP_PUT, SetTimer);
  _server.on(F("/timer"), HTTP_GET, GetTimeRemaining);

  _server.on(F("/time-schedule"), HTTP_GET, GetTimeSchedule);
  _server.on(F("/time-schedule"), HTTP_PUT, UpdateTimeSchedule);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _server.uri();
  message += "_server: ";
  message += (_server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += _server.args();
  message += "\n";

  for (uint8_t i = 0; i < _server.args(); i++) 
  {
    message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n";
  }

  _server.send(404, "text/plain", message);
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);

  if (WiFi.SSID() != _wifiName) 
  {
    Serial.println("Creating new connection to wifi");
    WiFi.begin(_wifiName, _wifiPassword);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  else
  {
    Serial.println("Using existing wifi settings...");
  }

 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(_wifiName);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  _server.onNotFound(handleNotFound);
  // Start server
  _server.begin();

  Serial.println("HTTP server started");
}