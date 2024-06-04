#include "LEDStrip.h"
#include <avr/pgmspace.h>

// const char str1[] PROGMEM = "Updating LED: ";
// const char str2[] PROGMEM = "LED updated";

LEDStrip::LEDStrip(uint8_t pin, uint16_t numLeds)
    : strip(numLeds, pin, NEO_GRB + NEO_KHZ800), lastLEDUpdateTime(0),
      ledUpdateInterval(50), ledState(0), currentLED(0), pattern(0) {}

void LEDStrip::begin() {
    strip.begin();
    strip.show();
    Serial.println("LED strip initialized");
}

void LEDStrip::controlLEDs() {
    unsigned long currentTime = millis();
    if (currentTime - lastLEDUpdateTime >= ledUpdateInterval) {
        lastLEDUpdateTime = currentTime;
        updateLEDs();
    }
}

void LEDStrip::updatePattern() {
    // Serial.println("Updating pattern");
    updateLEDs();
}

uint32_t LEDStrip::rainbow(int position) {
    position = position % 255;
    if (position < 85) {
        return strip.Color(position * 3, 255 - position * 3, 0);
    } else if (position < 170) {
        position -= 85;
        return strip.Color(255 - position * 3, 0, position * 3);
    } else {
        position -= 170;
        return strip.Color(0, position * 3, 255 - position * 3);
    }
}

uint32_t LEDStrip::blink(int position) {
    return (millis() / 500) % 2 == 0 ? strip.Color(255, 255, 255)
                                     : strip.Color(0, 0, 0);
}

uint32_t LEDStrip::colorWipe(uint32_t color, int position) { return color; }

uint32_t LEDStrip::theaterChase(uint32_t color, int position) {
    return (position % 3 == 0) ? color : strip.Color(0, 0, 0);
}

uint32_t LEDStrip::theaterChaseRainbow(int position) {
    return theaterChase(rainbow(position), position);
}

uint32_t LEDStrip::colorWipeRainbow(int position) {
    return colorWipe(rainbow(position), position);
}

void LEDStrip::setCustomColor(uint32_t color) {
    // Serial.print("Setting custom color: ");
    // Serial.println(color, HEX);
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

void LEDStrip::updateLEDs() {
    // Serial.print("Updating LEDs with pattern: ");
    // Serial.println(pattern);
    for (int i = 0; i < strip.numPixels(); i++) {
        uint32_t color;
        switch (pattern) {
        case 0:
            color = strip.Color(255, 0, 0); // Red
            break;
        case 1:
            color = strip.Color(0, 255, 0); // Green
            break;
        case 2:
            color = strip.Color(0, 0, 255); // Blue
            break;
        case 3:
            color = rainbow(i + currentLED); // Rainbow
            break;
        case 4:
            color = blink(i); // Blink
            break;
        case 5:
            color = colorWipe(strip.Color(255, 255, 0), i); // Yellow Wipe
            break;
        case 6:
            color = theaterChase(strip.Color(127, 127, 127),
                                 i + currentLED); // White Chase
            break;
        case 7:
            color =
                theaterChaseRainbow(i + currentLED); // Theater Chase Rainbow
            break;
        case 8:
            color = colorWipeRainbow(i + currentLED); // Color Wipe Rainbow
            break;
        default:
            color = strip.Color(0, 0, 0); // Off
            break;
        }
        strip.setPixelColor(i, color);
    }
    strip.show();
    currentLED++;
}

void LEDStrip::setPattern(uint8_t newPattern) {
    pattern = newPattern;
    currentLED = 0;
    // Serial.print("Pattern set to: ");
    // Serial.println(pattern);
}