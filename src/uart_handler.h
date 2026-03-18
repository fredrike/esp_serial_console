#pragma once

#include <ESPAsyncWebServer.h>
#include <Arduino.h>

// Shared UART processing function
// Reads UART data and sends complete lines via SSE
inline void processUARTData(Stream& serial, AsyncEventSource& events, String& lineBuffer) {
  while (serial.available()) {
    char c = serial.read();
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

// Shared server setup for UART terminal
inline void setupUARTServer(AsyncWebServer& server, AsyncEventSource& events, 
                           Stream& serial, const char* html_page) {
  // Serve main page
  server.on("/", HTTP_GET, [html_page](AsyncWebServerRequest *request) {
    request->send(200, "text/html", html_page);
  });

  // SSE events
  server.addHandler(&events);

  // Handle sending commands from web page
  server.on("/send", HTTP_GET, [&serial](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      serial.print(cmd);
    }
    request->send(200, "text/plain", "OK");
  });
}
