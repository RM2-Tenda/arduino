#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Adafruit_NeoPixel.h>

class LEDStrip {
  public:
    LEDStrip(uint8_t pin, uint16_t numLeds);
    void begin();
    void controlLEDs();
    void updatePattern();
    void setPattern(uint8_t newPattern);
    void setCustomColor(uint32_t color);

  private:
    Adafruit_NeoPixel strip;
    unsigned long lastLEDUpdateTime;
    uint16_t ledUpdateInterval;
    uint8_t ledState;
    uint8_t currentLED;
    uint8_t pattern;

    uint32_t rainbow(int position);
    uint32_t blink(int position);
    uint32_t colorWipe(uint32_t color, int position);
    uint32_t theaterChase(uint32_t color, int position);
    uint32_t theaterChaseRainbow(int position);
    uint32_t colorWipeRainbow(int position);
    void updateLEDs();
};

#endif