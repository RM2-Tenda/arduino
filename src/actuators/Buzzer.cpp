#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin) : pin(pin) {}

void Buzzer::begin() { pinMode(pin, OUTPUT); }

void Buzzer::on() { digitalWrite(pin, HIGH); }

void Buzzer::off() { digitalWrite(pin, LOW); }