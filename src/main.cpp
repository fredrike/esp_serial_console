#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

AsyncWebServer server(80);
String serialBuffer = "";

// HTML page with console and input
const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>
<head>
  <title>ESP32 Serial Console</title>
  <meta charset="UTF-8">
  <style>
    body {
      font-family: monospace;
      background-color: #1e1e1e;
      color: #00ff00;
      margin: 0;
      padding: 20px;
    }
    h2 {
      margin-top: 0;
      text-align: center;
    }
    #console {
      white-space: pre-wrap;
      background-color: #000;
      border: 2px solid #00ff00;
      padding: 10px;
      height: 400px;
      overflow-y: scroll;
      margin-bottom: 10px;
    }
    #input {
      width: 100%;
      padding: 10px;
      font-family: monospace;
      font-size: 1rem;
      color: #00ff00;
      background-color: #1e1e1e;
      border: 2px solid #00ff00;
      box-sizing: border-box;
    }
    #input:focus {
      outline: none;
      border-color: #ff0;
    }
  </style>
</head>
<body>
<h2>ESP32 Serial Console</h2>
<div id="console"></div>
<input type="text" id="input" placeholder="Type command and press Enter" autofocus>

<script>
const consoleDiv = document.getElementById('console');
const inputField = document.getElementById('input');

let eventSource = new EventSource('/serial');
eventSource.onmessage = function(e) {
  consoleDiv.textContent += e.data + '\\n';
  consoleDiv.scrollTop = consoleDiv.scrollHeight;
};

// Send input to ESP32
inputField.addEventListener("keypress", function(e) {
  if(e.key === "Enter" && inputField.value.trim() !== "") {
    fetch("/send?cmd=" + encodeURIComponent(inputField.value));
    inputField.value = "";
  }
});
</script>

</body>
</html>
)rawliteral";

void setup() {
Serial.begin(9600); // UART0, connected to PC
WiFi.begin(ssid, password);
Serial.println("Connecting to WiFi...");
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println();
Serial.print("Connected! IP: ");
Serial.println(WiFi.localIP());

// Serve main page
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
request->send_P(200, "text/html", index_html);
});

// Server-Sent Events endpoint for serial output
server.on("/serial", HTTP_GET, [](AsyncWebServerRequest *request){
AsyncWebServerResponse *response = request->beginChunkedResponse("text/event-stream", [](uint8_t *buffer, size_t maxLen, size_t &outLen, void *){
if (serialBuffer.length() > 0) {
outLen = serialBuffer.length();
memcpy(buffer, serialBuffer.c_str(), outLen);
serialBuffer = "";
} else {
outLen = 0;
}
});
response->addHeader("Cache-Control", "no-cache");
request->send(response);
});

// Endpoint to send commands to serial
server.on("/send", HTTP_GET, [](AsyncWebServerRequest *request){
if (request->hasParam("cmd")) {
String cmd = request->getParam("cmd")->value();
Serial.println(cmd); // send command to PC
}
request->send(200, "text/plain", "OK");
});

server.begin();
}

void loop() {
while (Serial.available()) {
char c = Serial.read();
serialBuffer += c;
}
}
