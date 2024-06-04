#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "actuators/Buzzer.h"
#include "sensors/DHTSensor.h"
#include "sensors/GasSensor.h"
#include "sensors/PresenceSensor.h"
#include "sensors/UVSensor.h"
#include <Arduino.h>
#include <time.h>

#define MAX_ALARMS 10

struct Alarm {
    String sensor;
    String condition;
    String comparison;
    float value;
    String days;
    String startTime;
    String endTime;
};

class TimeManager {
  public:
    void begin();
    void setTime(const String &timeString);
    void setAlarm(const String &sensor, const String &condition,
                  const String &comparison, float value, const String &days,
                  const String &startTime, const String &endTime);
    void checkAlarms(Buzzer &buzzer, DHTSensor &dht, GasSensor &gas,
                     UVSensor &uv, PresenceSensor &presence);

  private:
    Alarm alarms[MAX_ALARMS];
    int alarmCount;
    time_t currentTime;
    bool isCurrentTimeInRange(const String &startTime, const String &endTime);
    bool isCurrentDayInRange(const String &days);
};

#endif