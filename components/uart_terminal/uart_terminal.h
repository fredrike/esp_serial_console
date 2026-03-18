#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/web_server/web_server.h"
#include <ESPAsyncWebServer.h>

namespace esphome {
namespace uart_terminal {

class UARTTerminal : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  AsyncEventSource *events_{nullptr};
  std::string line_buffer_;
  
  const char* get_html_page();
};

}  // namespace uart_terminal
}  // namespace esphome
