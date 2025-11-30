#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

#include "config.h"  // contains WIFI_SSID, WIFI_PASS, HOSTNAME, OTA_PASSWORD

// HTTP/WebSocket server
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// UART buffer
constexpr size_t BUF_SIZE = 256;
uint8_t buf[BUF_SIZE];
size_t idx = 0;
unsigned long lastFlushMs = 0;
constexpr unsigned long FLUSH_INTERVAL = 20; // ms

// Minimal HTML page with xterm.js
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
<meta charset="utf-8" />
<title>ESP32 Terminal</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/xterm/css/xterm.css" />
<style>
html,body{height:100%;margin:0;background:#1e1e1e}
#terminal{width:100%;height:calc(100vh-48px)}
#inputbox{height:48px;display:flex;gap:8px;padding:8px;background:#111}
#cmd{flex:1;padding:8px;font-family:monospace;color:#0f0;background:#000;border:1px solid #0f0}
#send{padding:8px 12px}
</style>
</head>
<body>
<div id="terminal"></div>
<div id="inputbox">
<input id="cmd" placeholder="Type command and press Enter"/>
<button id="send">Send</button>
</div>
<script src="https://cdn.jsdelivr.net/npm/xterm/lib/xterm.js"></script>
<script>
const term = new Terminal({convertEol:false,cursorBlink:true,cols:80,rows:24,theme:{background:'#1e1e1e',foreground:'#00ff00'}});
term.open(document.getElementById('terminal'));
if(term.fit) term.fit();

let ws;
function startWS(){
    ws = new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+'/ws');
    ws.binaryType='arraybuffer';
    ws.onopen=()=>{term.writeln('[WebSocket connected]');};
    ws.onmessage=(ev)=>{
        if(ev.data instanceof ArrayBuffer){
            let s='';const u=new Uint8Array(ev.data);for(let i=0;i<u.length;i++) s+=String.fromCharCode(u[i]);
            term.write(s);
        } else {term.write(ev.data);}
    };
    ws.onclose=()=>{term.writeln('[WebSocket closed — reconnecting in 2s]'); setTimeout(startWS,2000);};
    ws.onerror=(e)=>{console.error('WS error',e); try{ws.close();}catch{}};
}
startWS();

function sendCmd(cmd){if(!ws||ws.readyState!==WebSocket.OPEN)return; ws.send(cmd+'\n');}
const cmdBox=document.getElementById('cmd');
cmdBox.addEventListener('keypress',e=>{if(e.key==='Enter'&&cmdBox.value.trim()!==''){sendCmd(cmdBox.value); cmdBox.value='';}});
document.getElementById('send').addEventListener('click',()=>{if(cmdBox.value.trim()!==''){sendCmd(cmdBox.value); cmdBox.value='';}});
window.addEventListener('resize',()=>{if(term.fit) term.fit();});
</script>
</body>
</html>
)rawliteral";

// WebSocket event (browser -> UART)
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if(type == WS_EVT_DATA){
        Serial.write(data, len);
    }
}

// Flush UART buffer to all WebSocket clients
void flush_buffer_binary() {
    if(idx == 0) return;
    size_t n = ws.count();
    for(size_t i=0;i<n;i++){
        AsyncWebSocketClient *client = ws.client(i);
        if(client && client->status()==WS_CONNECTED){
            client->binary(buf, idx);
        }
    }
    idx = 0;
}

void setup() {
    Serial.begin(9600);
    delay(50);

    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("[WIFI] Connecting...");
    while(WiFi.status()!=WL_CONNECTED){delay(250); Serial.print(".");}
    Serial.println();
    Serial.print("[WIFI] Connected, IP=");
    Serial.println(WiFi.localIP());

    // OTA
    ArduinoOTA.setHostname(HOSTNAME);
    if(strlen(OTA_PASSWORD)>0) ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();

    // HTTP
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200,"text/html",index_html);
    });

    // WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();
    Serial.println("[HTTP] server started");
}

void loop() {
    ArduinoOTA.handle();

    unsigned long now = millis();

    // Read UART and accumulate into buffer
    while(Serial.available() && idx < BUF_SIZE){
        int v = Serial.read();
        if(v<0) break;
        buf[idx++] = (uint8_t)v;
    }

    // Flush buffer either when full or after FLUSH_INTERVAL
    if(idx>0 && (idx>=BUF_SIZE || now-lastFlushMs>=FLUSH_INTERVAL)){
        flush_buffer_binary();
        lastFlushMs = now;
    }

    delay(1); // tiny yield
}
