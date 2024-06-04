#include "actuators/Buzzer.h"
#include "actuators/Fan.h"
#include "actuators/LEDStrip.h"
#include "communication/SerialComm.h"
#include "config/config.h"
#include "sensors/DHTSensor.h"
#include "sensors/GPSSensor.h"
#include "sensors/GasSensor.h"
#include "sensors/PresenceSensor.h"
#include "sensors/UVSensor.h"
#include "time/TimeManager.h"
#include <Arduino.h>
#include <MemoryFree.h> // Include the MemoryFree library
#include <avr/pgmspace.h>

SerialComm serialComm;
Buzzer buzzer(BUZZER_PIN);
Fan fan(FAN_1_PIN);
LEDStrip ledStrip(LED_PIN, NUM_LEDS);
DHTSensor dht(DHTPIN, DHTTYPE);
GPSSensor gps(GPS_TX_PIN);
GasSensor gas(GAS_SENSOR_AO, GAS_SENSOR_DO);
PresenceSensor presence(PRESENCE_PIN);
UVSensor uv(UV_SENSOR_PIN);
TimeManager timeManager;

const char str1[] PROGMEM = "Free memory: ";
const char str2[] PROGMEM = "Reading DHT sensor";
const char str3[] PROGMEM = "Reading GPS sensor";
const char str4[] PROGMEM = "Reading Gas sensor";
const char str5[] PROGMEM = "Reading Presence sensor";
const char str6[] PROGMEM = "Reading UV sensor";
const char str7[] PROGMEM = "Checking alarms";
const char str8[] PROGMEM = "Sending data";
const char str9[] PROGMEM = "Receiving commands";
const char str10[] PROGMEM = "Controlling LEDs";
const char str11[] PROGMEM = "Loop complete";

void setup() {
    serialComm.begin();
    buzzer.begin();
    fan.begin();
    ledStrip.begin();
    dht.begin();
    gps.begin();
    gas.begin();
    presence.begin();
    uv.begin();
    timeManager.begin();
    ledStrip.setPattern(0);
}

void loop() {
    timeManager.setTime("2024-06-04 10:00:00");
    static unsigned long lastDHTReadTime = 0;
    static unsigned long lastGPSReadTime = 0;
    static unsigned long lastGasReadTime = 0;
    static unsigned long lastPresenceReadTime = 0;
    static unsigned long lastUVReadTime = 0;
    static unsigned long lastSendDataTime = 0;
    static unsigned long lastReceiveCommandsTime = 0;
    static unsigned long lastLEDUpdateTime = 0;

    unsigned long currentTime = millis();

    if (currentTime - lastDHTReadTime >= 2000) {
        lastDHTReadTime = currentTime;
        dht.read();
    }

    if (currentTime - lastGPSReadTime >= 10000) {
        lastGPSReadTime = currentTime;
        gps.read();
    }

    if (currentTime - lastGasReadTime >= 500) {
        lastGasReadTime = currentTime;
        gas.read(buzzer);
    }

    if (currentTime - lastPresenceReadTime >=
        1000) { // Read Presence every 1 second
        lastPresenceReadTime = currentTime;
        presence.read();
    }

    if (currentTime - lastUVReadTime >= 1000) {
        lastUVReadTime = currentTime;
        uv.read();
    }

    if (currentTime - lastSendDataTime >= 1000) {
        lastSendDataTime = currentTime;
        serialComm.sendData(dht, gps, presence, gas, uv);
    }

    if (currentTime - lastReceiveCommandsTime >= 1000) {
        lastReceiveCommandsTime = currentTime;
        serialComm.receiveCommands(ledStrip, buzzer, fan, dht, gas, uv,
                                   timeManager);
    }

    if (currentTime - lastLEDUpdateTime >= 50) {
        lastLEDUpdateTime = currentTime;
        ledStrip.updatePattern();
    }

    timeManager.checkAlarms(buzzer, dht, gas, uv, presence);
}