#ifndef FAN_H
#define FAN_H

#include <Arduino.h>

class Fan {
  public:
    Fan(uint8_t pin);
    void begin();
    void on();
    void off();

  private:
    uint8_t pin;
};

#endif