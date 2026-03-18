#pragma once

// Shared HTML page for the web terminal interface
// Used by both PlatformIO (main.cpp) and ESPHome (uart_terminal.cpp)
const char TERMINAL_HTML[] PROGMEM = R"rawliteral(
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
<button onclick="sendCtrlD()">Send Ctrl-D</button>
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
function sendCtrlD() { sendCommand('\x04'); }
function sendCtrlL() { 
    term.clear();
    sendCommand('\x0c'); 
}
  
// Connect to SSE endpoint
const evtSource = new EventSource("/serial");
evtSource.onmessage = function(e) {
  // Write each line into xterm.js
  term.write(e.data + "\r\n");
};

// Handle user input
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
        // optionally handle Escape sequences or function keys
    }
    term.write(seq); // echo locally
    if(seq) fetch('/send?cmd='+encodeURIComponent(seq));
});

sendCtrlL();
</script>
</body>
</html>
)rawliteral";

// Legacy alias for backward compatibility
const char* const index_html = TERMINAL_HTML;
