#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <DHT.h>

class DHTSensor {
  public:
    DHTSensor(uint8_t pin, uint8_t type);
    void begin();
    void read();
    float getHumidity() const;
    float getTemperature() const;
    void setThresholds(float tempThreshold, float humThreshold);

  private:
    DHT dht;
    float humidity;
    float temperature;
    float tempThreshold;
    float humThreshold;
};

#endif