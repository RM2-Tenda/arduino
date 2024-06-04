#include "PresenceSensor.h"

PresenceSensor::PresenceSensor(uint8_t pin) : pin(pin), presence(0) {}

void PresenceSensor::begin() { pinMode(pin, INPUT); }

void PresenceSensor::read() { presence = digitalRead(pin); }

int PresenceSensor::getPresence() const { return presence; }