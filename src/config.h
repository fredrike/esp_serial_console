#pragma once

// WiFi
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// Hostname
#define HOSTNAME "tty-serial"
#define OTA_PASSWORD ""         // optional

// UART
#define SERIAL_BAUD       9600

#define MAX_READ 256           // chunk size - larger reduces sequence splitting

// Web server
#define HTTP_PORT       80
#define SSE_ENDPOINT    "/stream"

// Buffering
#define UART_READ_BUF   256
#define SSE_MAX_CLIENTS 8
