#ifndef GASSENSOR_H
#define GASSENSOR_H

#include "../actuators/Buzzer.h"
#include <Arduino.h>

class GasSensor {
  public:
    GasSensor(uint8_t aoPin, uint8_t doPin);
    void begin();
    void read();
    int getGasValue() const;
    bool isGasDetected() const;
    void setThreshold(int threshold);

  private:
    uint8_t aoPin;
    uint8_t doPin;
    int gasValue;
    bool gasDetected;
    int threshold;
};

#endif