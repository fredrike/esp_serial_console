#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include "config.h" // contains WIFI_SSID, WIFI_PASS, HOSTNAME, OTA_PASSWORD

AsyncWebServer server(80);
AsyncEventSource events("/serial"); // SSE endpoint

String serialBuffer = "";

// HTML page (minimal terminal interface)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Serial Console</title>
<style>
body { background:#1e1e1e; color:#00ff00; font-family: monospace; margin:0; padding:10px; }
#console { white-space: pre-wrap; height: 90vh; overflow-y: scroll; border: 1px solid #00ff00; padding:10px;}
</style>
</head>
<body>
<h2>ESP32 Serial Console</h2>
<div id="console"></div>
<script>
let consoleDiv = document.getElementById("console");
let evtSource = new EventSource("/serial");
evtSource.onmessage = function(e) {
  consoleDiv.textContent += e.data + "\\n";
  consoleDiv.scrollTop = consoleDiv.scrollHeight;
};
</script>
</body>
</html>
)rawliteral";

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
      Serial.println(cmd); // send to PC
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
