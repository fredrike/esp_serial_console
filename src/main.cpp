#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "web_interface.h"
#include "uart_handler.h"

AsyncWebServer server(80);
AsyncEventSource events("/serial");
String lineBuffer = "";

void setup()
{
  Serial.begin(SERIAL_BAUD);
  delay(50);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP=");
  Serial.println(WiFi.localIP());

  // OTA setup
  ArduinoOTA.setHostname(HOSTNAME);
  if (strlen(OTA_PASSWORD) > 0)
    ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();

  // Setup UART server with shared function
  setupUARTServer(server, events, Serial, TERMINAL_HTML);

  server.begin();
  Serial.println("Server started at /");
}

void loop()
{
  ArduinoOTA.handle();
  
  // Process UART data with shared function
  processUARTData(Serial, events, lineBuffer);
}
