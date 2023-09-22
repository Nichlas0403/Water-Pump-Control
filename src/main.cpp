#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "TimerService.h"
#include "DateTime.h"
#include "GPIOService.h"
#include "FlashService.h"
#include "MathService.h"

#define relayGPIO D7

//Core variables
String _wifiName = "";
String _wifiPassword = "";
unsigned long _timer;

//Services
ESP8266WebServer _server(80);
GPIOService _gpioService(relayGPIO);
FlashService _flashService;
TimerService _timerService;
MathService _mathService;

//FlashService Keys
String _wifiNameFlash = "wifiNameFlash";
String _wifiPasswordFlash = "wifiPassword";
String _cscsBaseUrl = "cscsBaseUrl";
String _getDateTimeRoute = "getDateTimeUrl";
String _sendTextRoute = "sendTextRoute";

//Function Definitions

void connectToWiFi();

void setup() 
{
  Serial.begin(9600);
  _gpioService.TurnRelayOff();

  _wifiName = _flashService.ReadFromFlash(_wifiNameFlash);
  _wifiPassword = _flashService.ReadFromFlash(_wifiPasswordFlash);

  connectToWiFi();
  

  //check if schedule needs to run 
  
}

void loop() 
{

  unsigned long currentTime = millis();

  _server.handleClient();
  delay(1);

  if(_timer != 0 && currentTime > _timer)
  {
    _gpioService.TurnRelayOff();
    _timer = 0;
  }

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

  DateTime timeRemaining = _timerService.CalculateTimeRemaining(_timer);

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

  if(_timer != 0)
  {
    _timer = 0;
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



  _gpioService.TurnRelayOn();

  _server.send(200);
}

void UpdateTimeSchedule()
{
  //Expected body:
  //{ "hoursStart" : 4, "minutesStart" : 22, "hoursEnd" : 17, "minutesEnd" : 45 }

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