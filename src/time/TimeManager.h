#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "actuators/Buzzer.h"
#include "actuators/Fan.h"
#include "sensors/DHTSensor.h"
#include "sensors/GasSensor.h"
#include "sensors/PresenceSensor.h"
#include "sensors/UVSensor.h"
#include <Arduino.h>
#include <time.h>

#define MAX_ALARMS 10

enum SensorType {
    TEMP = 0,
    HUM = 1,
    GAS = 2,
    UV = 3,
    MOTION = 4
};

enum ConditionType {
    VALUE = 0,
    DETECTED = 1
};

enum ComparisonType {
    ABOVE = 0,
    BELOW = 1,
    EQUAL = 2
};

struct Alarm {
    SensorType sensor;
    ConditionType condition;
    ComparisonType comparison;
    float value;
    uint8_t days; // Bitmask for days (0-6)
    uint16_t startTime; // Minutes from midnight
    uint16_t endTime;   // Minutes from midnight
};

class TimeManager {
  public:
    void begin();
    void setTime(const char* timeString);
    void setAlarm(SensorType sensor, ConditionType condition,
                  ComparisonType comparison, float value,
                  uint8_t days, uint16_t startTime,
                  uint16_t endTime);
    void checkAlarms(Buzzer &buzzer, Fan &fan, DHTSensor &dht, GasSensor &gas,
                     UVSensor &uv, PresenceSensor &presence);
    bool isAnyAlarmTriggered() const;

  private:
    Alarm alarms[MAX_ALARMS];
    int alarmCount;
    time_t currentTime;
    bool isCurrentTimeInRange(uint16_t startTime, uint16_t endTime);
    bool isCurrentDayInRange(uint8_t days);
    bool anyAlarmTriggered;
};

#endif