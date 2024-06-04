#include "Fan.h"

Fan::Fan(uint8_t pin) : pin(pin), state(false), commandState(false) {}

void Fan::begin() { pinMode(pin, OUTPUT); }

void Fan::on() {
    state = true;
    digitalWrite(pin, HIGH);
}

void Fan::off() {
    state = false;
    digitalWrite(pin, LOW);
}

void Fan::setState(bool state) {
    this->state = state;
    digitalWrite(pin, state ? HIGH : LOW);
}

bool Fan::getState() const { return state; }

void Fan::setCommandState(bool state) { this->commandState = state; }

bool Fan::getCommandState() const { return commandState; }