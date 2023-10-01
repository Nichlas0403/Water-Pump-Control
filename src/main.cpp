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
int _hourTimeScheduleStart;
int _hourTimeScheduleEnd;
unsigned long _timeScheduleStartMillis;
unsigned long _timeScheduleEndMillis;
bool _timeScheduleOn;
TimeScheduleModel _timeSchedule;

//FlashService Keys
String _wifiNameFlash = "wifiNameFlash";
String _wifiPasswordFlash = "wifiPassword";
String _cscsBaseUrlFlash = "cscsBaseUrl";
String _getDateTimeRouteFlash = "getDateTimeUrl";
String _sendTextRouteFlash = "sendTextRoute";
String _timeScheduleStartHourFlash = "timeScheduleStartHour";
String _timeScheduleEndHourFlash = "timeScheduleEndHour";

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

  _wifiName = _flashService.ReadFromFlash(_wifiNameFlash);
  _wifiPassword = _flashService.ReadFromFlash(_wifiPasswordFlash);

  connectToWiFi();

  String timeScheduleStart = _flashService.ReadFromFlash(_timeScheduleStartHourFlash);
  String timeScheduleEnd = _flashService.ReadFromFlash(_timeScheduleEndHourFlash);

  Serial.println();

  if(timeScheduleStart != "" && timeScheduleEnd != "")
  {
    GetTimeScheduleFromFlash();
    UpdateTimeScheduleStartAndEnd();
    _timeScheduleOn = true;
  }
}

void loop() 
{

  unsigned long currentTime = millis();

  _server.handleClient();
  delay(1);

  if(_timerOn && currentTime > _timer)
  {

    if(!_timeScheduleOn ||
      (_timeScheduleOn && currentTime <= _timeScheduleStartMillis))
    {
      _gpioService.TurnRelayOff();
    }

    _timer = 0;
    _timerOn = false;

  }

  if(_timeScheduleOn && !_gpioService.RelayState && currentTime >= _timeScheduleStartMillis)
  {
    _gpioService.TurnRelayOn();
  }
  else if(_timeScheduleOn && currentTime >= _timeScheduleEndMillis)
  {
    if(!_timerOn && _gpioService.RelayState)
    {
      _gpioService.TurnRelayOff();
    }

    UpdateTimeScheduleStartAndEnd();

  }
}


void UpdateTimeScheduleStartAndEnd()
{
  DateTimeModel currentDateTime = _httpService.GetDateTime();

  unsigned long millisValueWhenStart = _timeScheduleService.CalculateTimeUntil(currentDateTime.Hours, _hourTimeScheduleStart);
  millisValueWhenStart += millis();

  unsigned long minutesLeftInHourMillis = _mathService.ConvertMinutesToMillis(currentDateTime.Minutes);

  if(currentDateTime.Hours != _hourTimeScheduleStart)
  {
    millisValueWhenStart -= minutesLeftInHourMillis;
  }

  unsigned long millisValueWhenEnd = millisValueWhenStart + _timeScheduleService.CalculateTimeUntil(_hourTimeScheduleStart, _hourTimeScheduleEnd);

  if(currentDateTime.Hours == _hourTimeScheduleStart)
  {
    millisValueWhenEnd -= minutesLeftInHourMillis;
  }
  
  _timeScheduleStartMillis = millisValueWhenStart;
  _timeScheduleEndMillis = millisValueWhenEnd;  
}

void GetTimeScheduleFromFlash()
{
    _hourTimeScheduleStart = _flashService.ReadFromFlash(_timeScheduleStartHourFlash).toInt();
    _hourTimeScheduleEnd = _flashService.ReadFromFlash(_timeScheduleEndHourFlash).toInt();
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

  if(_hourTimeScheduleStart == 0 && _hourTimeScheduleEnd == 0)
  {
    _server.send(200, "text/json", "No Time Schedule has been set.");
  }

  String hourStart;
  String hourEnd;

  if(_hourTimeScheduleStart < 10)
  {
    hourStart = "0" + String(_hourTimeScheduleStart);
  }
  else
  {
    hourStart = String(_hourTimeScheduleStart);
  }

  if(_hourTimeScheduleEnd < 10)
  {
    hourEnd = "0" + String(_hourTimeScheduleEnd);
  }
  else
  {
    hourEnd = String(_hourTimeScheduleEnd);
  }

  _server.send(200, "text/json", hourStart + "-" + hourEnd);

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
    _timer = 0;
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

void StopTimer()
{
  unsigned long int currentTime = millis(); 

  if(!_timeScheduleOn ||
     (_timeScheduleOn && currentTime <= _timeScheduleStartMillis))
  {
    _gpioService.TurnRelayOff();
  }
     
  _timer = 0;
  _timerOn = false;

  _server.send(200);
}

void UpdateTimeSchedule()
{
  DynamicJsonDocument request(1024);
  
  deserializeJson(request, _server.arg("plain"));

  int hoursStart = request["hoursStart"];
  int hoursEnd = request["hoursEnd"];

  bool timerScheduleIsValid = _timeScheduleService.ValidateTimer(hoursStart, hoursEnd);

  if(!timerScheduleIsValid)
  {
    _server.send(400, "text/json", "Invalid time - Values must be between 0 and 23 and can not be equal.");
    return;
  }

  _hourTimeScheduleStart = hoursStart;
  _hourTimeScheduleEnd = hoursEnd;

  if(!_timerOn)
  {
    _gpioService.TurnRelayOff();
  }

  UpdateTimeScheduleStartAndEnd();

  _flashService.WriteToFlash(_timeScheduleStartHourFlash, String(hoursStart));
  _flashService.WriteToFlash(_timeScheduleEndHourFlash, String(hoursEnd));
  _timeScheduleOn = true;

  _server.send(200);

}

void StopTimeSchedule()
{
  _flashService.DeleteFromFlash(_timeScheduleStartHourFlash);
  _flashService.DeleteFromFlash(_timeScheduleEndHourFlash);

  _hourTimeScheduleStart = 0;
  _hourTimeScheduleEnd = 0;
  _timeScheduleOn = false;

  if(!_timerOn)
  {
    _gpioService.TurnRelayOff();
  }

  _server.send(200);

}

// Endpoints
void restServerRouting() 
{
  _server.on(F("/health-check"), HTTP_GET, HealthCheck);
  _server.on(F("/water-pump/state"), HTTP_GET, GetState);
  _server.on(F("/water-pump/on"), HTTP_PUT, TurnWaterPumpOn);
  _server.on(F("/water-pump/off"), HTTP_PUT, TurnWaterPumpOff);


  _server.on(F("/timer"), HTTP_PUT, SetTimer);
  _server.on(F("/timer"), HTTP_GET, GetTimeRemaining);
  _server.on(F("/timer"), HTTP_DELETE, StopTimer);

  _server.on(F("/time-schedule"), HTTP_GET, GetTimeSchedule);
  _server.on(F("/time-schedule"), HTTP_PUT, UpdateTimeSchedule);
  _server.on(F("/time-schedule"), HTTP_DELETE, StopTimeSchedule);
}

// Core server functionality

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