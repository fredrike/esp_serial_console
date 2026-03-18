#include "uart_terminal.h"
#include "esphome/core/log.h"

// Include shared web interface HTML
#include "web_interface.h"

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
  return TERMINAL_HTML;
}

}  // namespace uart_terminal
}  // namespace esphome
