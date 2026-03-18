# UART Terminal - ESPHome Component

This project provides an ESPHome custom component hosted on GitHub that delivers a web-based serial terminal interface for your ESP32.

## Features

- Web-based terminal using xterm.js
- Bidirectional UART communication
- Server-Sent Events (SSE) for real-time serial data streaming
- OTA updates via ESPHome
- Home Assistant API integration
- Available as external component from GitHub

## Installation

### Using as External Component (Recommended)

Reference the component directly from GitHub in your ESPHome YAML. See [example.yaml](example.yaml) for a complete configuration:

```yaml
external_components:
  - source:
      type: git
      url: git@github.com:fredrike/esp_serial_console.git
    components: [ uart_terminal ]

uart:
  id: uart_bus
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 115200

uart_terminal:
  uart_id: uart_bus
```

### Local Development

1. **Clone the repository**:
   ```bash
   git clone git@github.com:fredrike/esp_serial_console.git
   ```

2. **Install ESPHome** (if not already installed):
   ```bash
   pip install esphome
   ```

3. **Edit secrets.yaml** with your credentials:
   - WiFi SSID and password
   - API password
   - OTA password

4. **Compile and upload**:
   ```bash
   esphome run uart_terminal.yaml
   ```

## Usage

After the device boots and connects to WiFi, access the terminal at:
```
http://<device-ip>/terminal
```

Or find the device on your network at:
```
http://uart-terminal.local/terminal
```

## Configuration

### UART Settings

The default UART configuration uses:
- TX Pin: GPIO1
- RX Pin: GPIO3
- Baud Rate: 115200

To change these, edit the `uart:` section in `uart_terminal.yaml`:

```yaml
uart:
  id: uart_bus
  tx_pin: GPIO1    # Change as needed
  rx_pin: GPIO3    # Change as needed
  baud_rate: 115200  # Change as needed
```

### Web Server Port

The web server runs on port 80 by default. To change it:

```yaml
web_server:
  port: 8080  # Or any other port
```

## Terminal Controls

The web interface includes:
- **Ctrl-C button**: Send interrupt signal (0x03)
- **Ctrl-B button**: Send STX signal (0x02)
- **Clear Screen button**: Clear terminal display and send Form Feed (0x0C)

All keyboard input is forwarded to the UART, including:
- Arrow keys
- Backspace, Delete, Tab, Enter
- Home, End, PageUp, PageDown
- Regular text input

## Integration with Home Assistant

This component automatically integrates with Home Assistant via the ESPHome API. After adding the device to Home Assistant, you'll be able to:
- Monitor device status
- Trigger OTA updates
- View logs

## Troubleshooting

### Cannot connect to device
- Verify WiFi credentials in `secrets.yaml`
- Check your router for the device's IP address
- Try accessing via mDNS: `uart-terminal.local`

### Serial data not appearing
- Verify UART pins are correct for your board
- Check that the baud rate matches your serial device
- Ensure logger baud_rate is set to 0 (disabled) to avoid conflicts

### Web page not loading
- Make sure the web_server component is enabled
- Check that port 80 (or your custom port) is not blocked
- Verify device is connected to WiFi

## Component Structure

The component follows ESPHome's external component structure:

```
components/uart_terminal/
├── __init__.py           # Python configuration schema
├── uart_terminal.h       # C++ header file
└── uart_terminal.cpp     # C++ implementation
```

### Files:
1. **uart_terminal.yaml**: Example ESPHome configuration
2. **components/uart_terminal/**: The reusable component
   - `__init__.py`: ESPHome component configuration and validation
   - `uart_terminal.h`: Component header with class definition
   - `uart_terminal.cpp`: Implementation with web server and UART handling

## Development

To modify the component:

1. Edit the files in `components/uart_terminal/`
2. Test locally by using the component in your YAML
3. Commit and push to GitHub
4. Reference the updated version in your projects using `external_components`

## Differences from PlatformIO Version

The ESPHome version:
- Uses ESPHome's configuration system instead of `config.h`
- Integrates with Home Assistant automatically
- Uses ESPHome's UART component API
- Simplifies OTA and WiFi management through ESPHome framework
- Maintains the same web terminal functionality

## License

This project uses xterm.js (MIT License) for the terminal interface.
