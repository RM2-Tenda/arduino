#include "Fan.h"

Fan::Fan(uint8_t pin) : pin(pin) {}

void Fan::begin() { pinMode(pin, OUTPUT); }

void Fan::on() { digitalWrite(pin, HIGH); }

void Fan::off() { digitalWrite(pin, LOW); }