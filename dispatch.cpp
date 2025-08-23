#include <iostream>
#include <STM32_CAN.h>
#include "headers.hpp"

struct ComandaCAN {
    uint32_t id;
    uint8_t cmd;
    void (*handler)();
  };
  
  void actiune11() { Serial.println("Comanda 0x11 detectată"); }
  void actiune22() { Serial.println("Comanda 0x22 detectată"); }
  
  ComandaCAN comenzi[] = {
    {0x123, 0x11, actiune11},
    {0x123, 0x22, actiune22}
  };
  
  void proceseaza(const CAN_message_t& rx) {
    for (auto &c : comenzi) {
      if (rx.id == c.id && rx.len > 0 && rx.buf[0] == c.cmd) {
        c.handler();
        return;
      }
    }
    Serial.println("Comandă necunoscută");
  }
  