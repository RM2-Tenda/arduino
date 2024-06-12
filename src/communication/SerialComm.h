#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include "actuators/Buzzer.h"
#include "actuators/Fan.h"
#include "actuators/LEDStrip.h"
#include "sensors/DHTSensor.h"
#include "sensors/GPSSensor.h"
#include "sensors/GasSensor.h"
#include "sensors/PresenceSensor.h"
#include "sensors/UVSensor.h"
#include "time/TimeManager.h"
#include <Arduino.h>
#include <ArduinoJson.h>

class SerialComm {
  public:
    void begin();
    void sendData(const DHTSensor &dht, const GPSSensor &gps,
                  const PresenceSensor &presence, const GasSensor &gas,
                  const UVSensor &uv);
    void receiveCommands(LEDStrip &ledStrip, Buzzer &buzzer, Fan &fan,
                         DHTSensor &dht, GasSensor &gas, UVSensor &uv,
                         TimeManager &timeManager);

  private:
    void processCommand(const char* command, LEDStrip &ledStrip,
                        Buzzer &buzzer, Fan &fan, DHTSensor &dht,
                        GasSensor &gas, UVSensor &uv, TimeManager &timeManager);
    const char* getNextParam(const char* params, int &startIndex);
    unsigned long lastSendTime;
    const unsigned long sendInterval = 1000;
};

#endif