#ifndef FAN_H
#define FAN_H

#include <Arduino.h>

class Fan {
  public:
    Fan(uint8_t pin);
    void begin();
    void on();
    void off();
    void setState(bool state);
    bool getState() const;
    void setCommandState(bool state);
    bool getCommandState() const;

  private:
    uint8_t pin;
    bool state;
    bool commandState;
};

#endif