#include "TimeManager.h"
#include "sensors/PresenceSensor.h"

void TimeManager::begin() {
    alarmCount = 0;
    currentTime = 0;
}

void TimeManager::setTime(const char* timeString) {
    struct tm timeinfo;
    int year, month, day, hour, minute, second;
    sscanf(timeString, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second);
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    currentTime = mktime(&timeinfo);
}

void TimeManager::setAlarm(SensorType sensor, ConditionType condition,
                           ComparisonType comparison, float value,
                           uint8_t days, uint16_t startTime,
                           uint16_t endTime) {
    if (alarmCount < MAX_ALARMS) {
        alarms[alarmCount] = {sensor, condition, comparison, value, days, startTime, endTime};
        alarmCount++;
    } else {
        Serial.println("Max number of alarms reached");
    }
}

void TimeManager::checkAlarms(Buzzer &buzzer, Fan &fan, DHTSensor &dht,
                              GasSensor &gas, UVSensor &uv,
                              PresenceSensor &presence) {
    anyAlarmTriggered = false;
    for (int i = 0; i < alarmCount; i++) {
        if (isCurrentDayInRange(alarms[i].days) &&
            isCurrentTimeInRange(alarms[i].startTime, alarms[i].endTime)) {
            bool trigger = false;
            switch (alarms[i].sensor) {
                case TEMP:
                    if (alarms[i].condition == VALUE) {
                        if ((alarms[i].comparison == ABOVE && dht.getTemperature() > alarms[i].value) ||
                            (alarms[i].comparison == BELOW && dht.getTemperature() < alarms[i].value) ||
                            (alarms[i].comparison == EQUAL && dht.getTemperature() == alarms[i].value)) {
                            trigger = true;
                        }
                    }
                    break;
                case HUM:
                    if (alarms[i].condition == VALUE) {
                        if ((alarms[i].comparison == ABOVE && dht.getHumidity() > alarms[i].value) ||
                            (alarms[i].comparison == BELOW && dht.getHumidity() < alarms[i].value) ||
                            (alarms[i].comparison == EQUAL && dht.getHumidity() == alarms[i].value)) {
                            trigger = true;
                        }
                    }
                    break;
                case GAS:
                    if ((alarms[i].condition == VALUE && 
                         ((alarms[i].comparison == ABOVE && gas.getGasValue() > alarms[i].value) ||
                          (alarms[i].comparison == BELOW && gas.getGasValue() < alarms[i].value) ||
                          (alarms[i].comparison == EQUAL && gas.getGasValue() == alarms[i].value))) ||
                        (alarms[i].condition == DETECTED && gas.isGasDetected())) {
                        trigger = true;
                    }
                    break;
                case UV:
                    if (alarms[i].condition == VALUE) {
                        if ((alarms[i].comparison == ABOVE && uv.getUVValue() > alarms[i].value) ||
                            (alarms[i].comparison == BELOW && uv.getUVValue() < alarms[i].value) ||
                            (alarms[i].comparison == EQUAL && uv.getUVValue() == alarms[i].value)) {
                            trigger = true;
                        }
                    }
                    break;
                case MOTION:
                    if (alarms[i].condition == DETECTED && presence.getPresence()) {
                        trigger = true;
                    }
                    break;
            }

            if (trigger) {
                buzzer.setState(true);
                fan.setState(true);
                anyAlarmTriggered = true;
            }
        }
    }
}

bool TimeManager::isAnyAlarmTriggered() const { return anyAlarmTriggered; }

bool TimeManager::isCurrentTimeInRange(uint16_t startTime, uint16_t endTime) {
    if ((startTime == 0 && endTime == 0) || (startTime == 0 && endTime == 2359) || (startTime == 2359 && endTime == 0) || startTime == endTime) {
        return true;
    }

    time_t now = currentTime + millis() / 1000;
    struct tm *timeinfo = localtime(&now);
    int currentMinutes = timeinfo->tm_hour * 60 + timeinfo->tm_min;

    if (startTime <= endTime) {
        return currentMinutes >= startTime && currentMinutes <= endTime;
    } else {
        return currentMinutes >= startTime || currentMinutes <= endTime;
    }
}

bool TimeManager::isCurrentDayInRange(uint8_t days) {
    if (days == 0) {
        return true;
    }

    time_t now = currentTime + millis() / 1000;
    struct tm *timeinfo = localtime(&now);
    int currentDay = timeinfo->tm_wday;
    return days & (1 << currentDay);
}