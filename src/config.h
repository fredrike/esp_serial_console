#pragma once

// WiFi credentials
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASS       "YOUR_WIFI_PASSWORD"

// Device settings
#define HOSTNAME        "tty-serial"
#define OTA_PASSWORD    ""  // Leave empty for no password

// UART configuration
#define SERIAL_BAUD     9600
#define MAX_READ        256  // chunk size - larger reduces sequence splitting
