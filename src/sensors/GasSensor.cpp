#include "GasSensor.h"

GasSensor::GasSensor(uint8_t aoPin, uint8_t doPin)
    : aoPin(aoPin), doPin(doPin), gasValue(0), gasDetected(false),
      threshold(0) {}

void GasSensor::begin() { pinMode(doPin, INPUT); }

void GasSensor::read() {
    gasValue = analogRead(aoPin);
    gasDetected = !digitalRead(doPin);
}

int GasSensor::getGasValue() const { return gasValue; }

bool GasSensor::isGasDetected() const { return gasDetected; }

void GasSensor::setThreshold(int threshold) { this->threshold = threshold; }