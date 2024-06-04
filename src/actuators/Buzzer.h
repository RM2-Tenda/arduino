#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  public:
    Buzzer(uint8_t pin);
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