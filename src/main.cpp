#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "web_interface.h"

AsyncWebServer server(80);
AsyncEventSource events("/serial");

void setup() {
  Serial.begin(SERIAL_BAUD); // UART0 connected to PC
  delay(50);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP=");
  Serial.println(WiFi.localIP());

  // OTA setup
  ArduinoOTA.setHostname(HOSTNAME);
  if (strlen(OTA_PASSWORD) > 0) ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();

  // Serve main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  // SSE events
  server.addHandler(&events);

  // Optional: handle sending commands from web page
  server.on("/send", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      Serial.print(cmd); // send to PC
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server started at /");
}

void loop() {
  ArduinoOTA.handle();

  // Read UART and append to buffer
  static String lineBuffer = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (lineBuffer.length() > 0) {
        events.send(lineBuffer.c_str());
        lineBuffer = "";
      }
    } else {
      lineBuffer += c;
    }
  }
}
