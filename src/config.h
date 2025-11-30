#pragma once

// WiFi
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// Hostname
#define DEVICE_HOSTNAME "uart-terminal"

// UART
#define UART_BAUD       115200

// Web server
#define HTTP_PORT       80
#define SSE_ENDPOINT    "/stream"

// Buffering
#define UART_READ_BUF   256
#define SSE_MAX_CLIENTS 8
