#include "UVSensor.h"

UVSensor::UVSensor(uint8_t pin) : pin(pin), uvValue(0), threshold(0) {}

void UVSensor::begin() { pinMode(pin, INPUT); }

void UVSensor::read() { uvValue = analogRead(pin); }

int UVSensor::getUVValue() const { return uvValue; }

void UVSensor::setThreshold(int threshold) { this->threshold = threshold; }