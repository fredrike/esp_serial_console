# ESP32 Serial Console

A web-based serial console for ESP32 that provides remote access to UART serial communication through a browser interface.

**Two ways to use this project:**
- 🎯 **ESPHome Component** (recommended) - Install as external component from GitHub for Home Assistant integration
- ⚡ **PlatformIO/Arduino** - Traditional Arduino framework for standalone use

## Features

- 🌐 **Web-based terminal** - Access serial console from any browser
- 📡 **Real-time streaming** - Uses Server-Sent Events (SSE) for live serial data
- ⌨️ **Full terminal support** - Powered by xterm.js with VT100 escape sequences
- 🔄 **OTA updates** - Update firmware over-the-air
- 🎨 **Retro terminal UI** - Green-on-black terminal theme
- 🎮 **Control buttons** - Quick access to Ctrl-C, Ctrl-B, and clear screen

## Hardware Requirements

- ESP32 board (tested on Wemos D1 Mini32)
- USB cable for initial programming
- Device with UART interface connected to ESP32

---

## PlatformIO/Arduino Version

For standalone use without Home Assistant.

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/fredrike/esp_serial_console.git
   cd esp_serial_console
   ```

2. **Setup virtual environment** (recommended):
   ```bash
   python3 -m venv venv
   source venv/bin/activate  # or: source activate.sh (includes SSL fixes)
   pip install -r requirements.txt
   ```

3. **Configure WiFi** in [src/config.h](src/config.h):
   ```cpp
   #define WIFI_SSID       "YOUR_WIFI_SSID"
   #define WIFI_PASS       "YOUR_WIFI_PASSWORD"
   #define HOSTNAME        "tty-serial"
   #define SERIAL_BAUD     9600
   ```

4. **Build and upload**:
   ```bash
   source activate.sh  # Sets up environment + SSL fixes
   pio run --target upload
   ```

**See [HOW_TO_COMPILE.md](HOW_TO_COMPILE.md) for troubleshooting.**

---

## ESPHome Version (Recommended)

### Quick Start

Reference this component directly in your ESPHome YAML configuration. See [example.yaml](example.yaml) for complete configuration:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/fredrike/esp_serial_console.git
    components: [ uart_terminal ]

uart:
  id: uart_bus
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 115200

uart_terminal:
  uart_id: uart_bus
```

### Installation

1. **Install ESPHome** (if not already installed):
   ```bash
   pip install esphome
   ```

2. **Create your configuration** using the example above or copy [uart_terminal.yaml](uart_terminal.yaml)

3. **Edit secrets.yaml** with your credentials:
   ```yaml
   wifi_ssid: "YOUR_WIFI_SSID"
   wifi_password: "YOUR_WIFI_PASSWORD"
   api_password: "YOUR_API_PASSWORD"
   ota_password: "YOUR_OTA_PASSWORD"
   ```

4. **Compile and upload**:
   ```bash
   # First time: install ESPHome if needed
   pip install esphome
   
   # Compile and upload
   esphome run your_config.yaml
   ```

### Usage

After the device boots and connects to WiFi:
- Access terminal at: `http://<device-ip>/terminal`
- Or via mDNS: `http://uart-terminal.local/terminal`

### Home Assistant Integration

This component automatically integrates with Home Assistant via the ESPHome API:
- Monitor device status
- Trigger OTA updates
- View logs
- No additional configuration needed

### Configuration Options

**UART Settings:**
```yaml
uart:
  id: uart_bus
  tx_pin: GPIO1      # Change as needed
  rx_pin: GPIO3      # Change as needed
  baud_rate: 115200  # Change as needed
```

**Web Server Port:**
```yaml
web_server:
  port: 8080  # Default is 80
```

---

## PlatformIO/Arduino Version

For standalone use without Home Assistant integration.

### Installation

1. **Clone the repository**:
   ```bash
   git clone git@github.com:fredrike/esp_serial_console.git
   cd esp_serial_console
   ```

2. **Configure WiFi credentials** in [src/config.h](src/config.h):
   ```cpp
   #define WIFI_SSID       "YOUR_WIFI_SSID"
   #define WIFI_PASS       "YOUR_WIFI_PASSWORD"
   #define HOSTNAME        "uart-terminal"
   #define SERIAL_BAUD     115200
   ```

3. **Build and upload** using PlatformIO:
   ```bash
   # Add pio to PATH if needed
   export PATH=$PATH:~/.local/bin
   
   # Upgrade to latest version (recommended)
   pip install -U platformio
   
   # Build (auto-installs ESP32 platform on first run)
   pio run
   
   # Upload
   pio run --target upload
   ```

   Or use the PlatformIO IDE upload button.

4. **Monitor serial output** (optional):
   ```bash
   pio device monitor
   ```

### Usage

1. Power on the ESP32
2. Wait for it to connect to WiFi (check serial monitor for IP address)
3. Open web browser and navigate to:
   - `http://<ESP32_IP_ADDRESS>` or
   - `http://uart-terminal.local` (if mDNS is working)
4. The terminal interface will appear

---

## Terminal Controls

The web interface includes buttons and full keyboard support:

**Control Buttons:**
- **Ctrl-C** - Send interrupt signal (ASCII 0x03)
- **Ctrl-B** - Send STX signal (ASCII 0x02)
- **Clear Screen** - Clear terminal and send Form Feed (ASCII 0x0C)

**Keyboard Support:**
- Standard ASCII characters
- Arrow keys (Up, Down, Left, Right)
- Backspace, Delete, Tab, Enter
- Home, End, Page Up, Page Down
- All input is forwarded to the UART

## UART Configuration

**Default Settings:**
- **Baud rate**: 115200
- **TX Pin**: GPIO1 (UART0)
- **RX Pin**: GPIO3 (UART0)

For ESPHome, configure in YAML. For PlatformIO, edit [src/config.h](src/config.h).

## Project Structure

```
esp_serial_console/
├── components/
│   └── uart_terminal/        # ESPHome component
│       ├── __init__.py        # Configuration schema
│       ├── uart_terminal.h    # Header file
│       └── uart_terminal.cpp  # Implementation
├── src/
│   ├── main.cpp              # PlatformIO main code
│   ├── config.h              # PlatformIO configuration
│   └── web_interface.h       # HTML interface
├── platformio.ini            # PlatformIO configuration
├── uart_terminal.yaml        # Example ESPHome config
├── example.yaml              # Minimal ESPHome example
└── README.md                 # This file
```

## Dependencies

**ESPHome Version:**
- [ESPHome](https://esphome.io/) framework
- [xterm.js](https://xtermjs.org/) (loaded from CDN)
- Automatically includes: AsyncTCP, ESPAsyncWebServer

**PlatformIO Version:**
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
- [xterm.js](https://xtermjs.org/) (loaded from CDN)

## Troubleshooting

### ESPHome Version

**Cannot connect to device:**
- Verify WiFi credentials in `secrets.yaml`
- Check router for device's IP address
- Try mDNS: `uart-terminal.local`

**Serial data not appearing:**
- Verify UART pins match your board
- Check baud rate matches connected device
- Ensure `logger: baud_rate: 0` to avoid UART conflicts

**Web page not loading:**
- Verify `web_server:` is enabled in YAML
- Check firewall/network settings
- Try accessing by IP instead of hostname

### PlatformIO Version

**ESP32 won't connect to WiFi:**
- Double-check SSID and password in [src/config.h](src/config.h)
- Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check serial monitor for connection status

**Can't access web interface:**
- Find IP address from serial monitor
- Try accessing by IP instead of hostname
- Verify device is on same network

**Serial data not appearing:**
- Verify UART baud rate in config
- Check TX/RX connections
- Ensure connected device is sending data

### Multiple USB Serial Devices

**If you have multiple USB serial devices** (e.g., SkyConnect ZigBee adapter) connected to the same system, you may need to change the USB device identifiers to prevent port conflicts. See [documentation/notes.md](documentation/notes.md) for detailed instructions on:
- Programming unique USB vendor/product IDs
- Setting up udev rules for persistent device names
- Using Docker to program USB devices on TrueNAS

## Additional Documentation

- **[SUMMARY.md](SUMMARY.md)** - Complete setup summary with SSL fixes ⭐
- **[SSL_FIX.md](SSL_FIX.md)** - How to fix SSL certificate issues in WSL
- **[HOW_TO_COMPILE.md](HOW_TO_COMPILE.md)** - Comprehensive compilation guide
- **[activate.sh](activate.sh)** - One-command environment setup script
- **[documentation/notes.md](documentation/notes.md)** - USB serial device ID configuration
- **[documentation/truenas.md](documentation/truenas.md)** - TrueNAS-specific fixes

## Contributing

### Modifying the ESPHome Component

1. Edit files in `components/uart_terminal/`
2. Test locally or direct from your fork
3. Commit and push to your repository
4. Reference your repo in `external_components`

### Modifying the PlatformIO Version

1. Edit [src/main.cpp](src/main.cpp) for logic
2. Edit [src/web_interface.h](src/web_interface.h) for HTML/JS
3. Edit [src/config.h](src/config.h) for settings
4. Build and upload with `pio run --target upload`

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

This project uses [xterm.js](https://xtermjs.org/) (MIT License) for the terminal interface.
