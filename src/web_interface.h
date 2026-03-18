#pragma once

// HTML page for the web terminal interface
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
.container { max-width: 800px; margin: 20px; }
.buttons { margin-bottom: 10px; }
button { padding: 10px 15px; margin-right: 5px; cursor: pointer; }
</style>
</head>
<body>
<div class="container">
<div id="terminal"></div>
<div class="buttons">
<button onclick="sendCtrlC()">Send Ctrl-C</button><button onclick="sendCtrlB()">Send Ctrl-B</button>
</div>
</div>

<script src="https://cdn.jsdelivr.net/npm/xterm/lib/xterm.js"></script>
<script src="https://cdn.jsdelivr.net/npm/xterm/lib/addons/fit/fit.js"></script>
<script>
const term = new Terminal({
  convertEol: false,
  cursorBlink: true,
  cols: 80,
  rows: 36,
  theme: { background:'#1e1e1e', foreground:'#00ff00' }
});
term.open(document.getElementById('terminal'));
if (term.fit) term.fit();

// Function to send command to the server and echo locally
function sendCommand(seq) {
    term.write(seq); // echo locally in the terminal
    if (seq) {
        // This is the fetch call that connects to your backend
        fetch('/send?cmd=' + encodeURIComponent(seq))
            .catch(error => console.error('Error sending command:', error));
    }
}

// Function specifically for the "Ctrl-C" button (ASCII 3, ETX)
function sendCtrlC() {
    sendCommand('\x03');
}

// Function specifically for the "Ctrl-B" button (ASCII 2, STX)
function sendCtrlB() {
    sendCommand('\x02');
}
// Function to send the clear screen (Ctrl-L) command (ASCII 12, FF)
function sendCtrlL() {
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
