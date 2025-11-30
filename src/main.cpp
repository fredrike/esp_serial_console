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
<title>ESP32 Terminal</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/xterm/css/xterm.css" />
<style>
html, body { height:100%; margin:0; background:#1e1e1e; }
#terminal { width:100%; height:100%; }
</style>
</head>
<body>
<div id="terminal"></div>

<script src="https://cdn.jsdelivr.net/npm/xterm/lib/xterm.js"></script>
<script src="https://cdn.jsdelivr.net/npm/xterm/lib/addons/fit/fit.js"></script>
<script>
const term = new Terminal({
  convertEol: false,
  cursorBlink: true,
  cols: 80,
  rows: 24,
  theme: { background:'#1e1e1e', foreground:'#00ff00' }
});
term.open(document.getElementById('terminal'));
if (term.fit) term.fit();

// Connect to SSE endpoint
const evtSource = new EventSource("/serial");
evtSource.onmessage = function(e) {
  // Write each line into xterm.js
  term.write(e.data + "\r\n");
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
