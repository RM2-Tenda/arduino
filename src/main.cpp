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
#include <MemoryFree.h>
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

void setup() {
    Serial.begin(9600);
    Serial.println(F("Setup started"));
    Serial.print(F("Free memory: "));
    Serial.println(freeMemory());

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

    Serial.println(F("Setup complete"));
}

void loop() {
    static bool firstLoop = true;
    if (firstLoop) {
        Serial.println(F("First loop iteration"));
        firstLoop = false;
    }

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
        gas.read();
    }

    if (currentTime - lastPresenceReadTime >= 1000) {
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
        // Serial.println(F("Checking for received commands...")); // Debug print
        serialComm.receiveCommands(ledStrip, buzzer, fan, dht, gas, uv, timeManager);
    }

    if (currentTime - lastLEDUpdateTime >= 50) {
        lastLEDUpdateTime = currentTime;
        ledStrip.updatePattern();
    }

    if (gas.isGasDetected()) {
        buzzer.setState(true);
        fan.setState(true);
    } else {
        fan.setState(false);
    }
    timeManager.checkAlarms(buzzer, fan, dht, gas, uv, presence);

    if (!gas.isGasDetected() && !timeManager.isAnyAlarmTriggered()) {
        buzzer.setState(false);
        fan.setState(false);
    }
}