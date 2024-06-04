#ifndef UVSENSOR_H
#define UVSENSOR_H

#include <Arduino.h>

class UVSensor {
  public:
    UVSensor(uint8_t pin);
    void begin();
    void read();
    int getUVValue() const;
    void setThreshold(int threshold);

  private:
    uint8_t pin;
    int uvValue;
    int threshold;
};

#endif