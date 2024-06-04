#ifndef PRESENCESENSOR_H
#define PRESENCESENSOR_H

#include <Arduino.h>

class PresenceSensor {
  public:
    PresenceSensor(uint8_t pin);
    void begin();
    void read();
    int getPresence() const;

  private:
    uint8_t pin;
    int presence;
};

#endif