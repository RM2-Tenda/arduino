#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin) : pin(pin), state(false), commandState(false) {}

void Buzzer::begin() { pinMode(pin, OUTPUT); }

void Buzzer::on() {
    state = true;
    digitalWrite(pin, HIGH);
}

void Buzzer::off() {
    state = false;
    digitalWrite(pin, LOW);
}

void Buzzer::setState(bool state) {
    this->state = state;
    digitalWrite(pin, state ? HIGH : LOW);
}

bool Buzzer::getState() const { return state; }

void Buzzer::setCommandState(bool state) { this->commandState = state; }

bool Buzzer::getCommandState() const { return commandState; }