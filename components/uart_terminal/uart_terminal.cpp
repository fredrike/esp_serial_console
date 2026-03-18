#include "uart_terminal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace uart_terminal {

static const char *const TAG = "uart_terminal";

void UARTTerminal::setup() {
  ESP_LOGCONFIG(TAG, "Setting up UART Terminal...");
  
  // Get the web server instance
  auto *web_server = web_server::global_web_server;
  if (web_server == nullptr) {
    ESP_LOGE(TAG, "Web server not initialized");
    this->mark_failed();
    return;
  }

  auto *server = web_server->get_server();
  
  // Initialize SSE handler
  this->events_ = new AsyncEventSource("/serial");
  server->addHandler(this->events_);

  // Serve main terminal page
  server->on("/terminal", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/html", this->get_html_page());
  });

  // Handle sending commands to UART
  server->on("/send", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      this->write_str(cmd.c_str());
    }
    request->send(200, "text/plain", "OK");
  });

  ESP_LOGI(TAG, "Serial terminal available at /terminal");
}

void UARTTerminal::loop() {
  // Read from UART and send to SSE clients
  while (this->available()) {
    uint8_t data;
    this->read_byte(&data);
    
    char c = (char)data;
    if (c == '\n' || c == '\r') {
      if (this->line_buffer_.length() > 0) {
        this->events_->send(this->line_buffer_.c_str());
        this->line_buffer_ = "";
      }
    } else {
      this->line_buffer_ += c;
    }
  }
}

void UARTTerminal::dump_config() {
  ESP_LOGCONFIG(TAG, "UART Terminal:");
  ESP_LOGCONFIG(TAG, "  Web interface: /terminal");
  ESP_LOGCONFIG(TAG, "  SSE endpoint: /serial");
}

const char* UARTTerminal::get_html_page() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Terminal</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/xterm/css/xterm.css" />
<style>
html, body { height:100%; margin:0; background:#1e1e1e; }
#terminal { width:100%; height:calc(100% - 60px); }
.container { max-width: 100%; margin: 0; height: 100%; display: flex; flex-direction: column; }
.buttons { padding: 10px; background: #2d2d2d; }
button { padding: 10px 15px; margin-right: 5px; cursor: pointer; background: #4CAF50; color: white; border: none; border-radius: 4px; }
button:hover { background: #45a049; }
</style>
</head>
<body>
<div class="container">
<div class="buttons">
<button onclick="sendCtrlC()">Send Ctrl-C</button>
<button onclick="sendCtrlB()">Send Ctrl-B</button>
<button onclick="sendCtrlL()">Clear Screen</button>
</div>
<div id="terminal"></div>
</div>

<script src="https://cdn.jsdelivr.net/npm/xterm/lib/xterm.js"></script>
<script>
const term = new Terminal({
  convertEol: false,
  cursorBlink: true,
  cols: 80,
  rows: 36,
  theme: { background:'#1e1e1e', foreground:'#00ff00' }
});
term.open(document.getElementById('terminal'));

function sendCommand(seq) {
    term.write(seq);
    if (seq) {
        fetch('/send?cmd=' + encodeURIComponent(seq))
            .catch(error => console.error('Error sending command:', error));
    }
}

function sendCtrlC() { sendCommand('\x03'); }
function sendCtrlB() { sendCommand('\x02'); }
function sendCtrlL() { 
    term.clear();
    sendCommand('\x0c'); 
}

const evtSource = new EventSource("/serial");
evtSource.onmessage = function(e) {
  term.write(e.data + "\r\n");
};

term.onKey(e => {
    const ev = e.domEvent;
    let seq = e.key;
    switch (ev.key) {
        case "ArrowUp":    seq = "\x1b[A"; break;
        case "ArrowDown":  seq = "\x1b[B"; break;
        case "ArrowRight": seq = "\x1b[C"; break;
        case "ArrowLeft":  seq = "\x1b[D"; break;
        case "Backspace":  seq = "\x08"; break;
        case "Enter":      seq = "\r"; break;
        case "Tab":        seq = "\t"; break;
        case "Home":       seq = "\x1b[H"; break;
        case "End":        seq = "\x1b[F"; break;
        case "PageUp":     seq = "\x1b[5~"; break;
        case "PageDown":   seq = "\x1b[6~"; break;
        case "Delete":     seq = "\x1b[3~"; break;
    }
    term.write(seq);
    if(seq) fetch('/send?cmd='+encodeURIComponent(seq));
});

sendCtrlL();
</script>
</body>
</html>
)rawliteral";
}

}  // namespace uart_terminal
}  // namespace esphome
