#include "DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type)
    : dht(pin, type), humidity(NAN), temperature(NAN), tempThreshold(NAN),
      humThreshold(NAN) {}

void DHTSensor::begin() { dht.begin(); }

void DHTSensor::read() {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
}

float DHTSensor::getHumidity() const { return humidity; }

float DHTSensor::getTemperature() const { return temperature; }

void DHTSensor::setThresholds(float tempThreshold, float humThreshold) {
    this->tempThreshold = tempThreshold;
    this->humThreshold = humThreshold;
}