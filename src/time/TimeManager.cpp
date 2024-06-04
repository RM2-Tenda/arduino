#include "TimeManager.h"
#include "sensors/PresenceSensor.h"

void TimeManager::begin() {
    alarmCount = 0;
    currentTime = 0;
}

void TimeManager::setTime(const String &timeString) {
    struct tm timeinfo;
    timeinfo.tm_year = timeString.substring(0, 4).toInt() - 1900;
    timeinfo.tm_mon = timeString.substring(5, 7).toInt() - 1;
    timeinfo.tm_mday = timeString.substring(8, 10).toInt();
    timeinfo.tm_hour = timeString.substring(11, 13).toInt();
    timeinfo.tm_min = timeString.substring(14, 16).toInt();
    timeinfo.tm_sec = timeString.substring(17, 19).toInt();
    currentTime = mktime(&timeinfo);
}

void TimeManager::setAlarm(const String &sensor, const String &condition,
                           const String &comparison, float value,
                           const String &days, const String &startTime,
                           const String &endTime) {
    if (alarmCount < MAX_ALARMS) {
        alarms[alarmCount] = {sensor, condition, comparison, value,
                              days,   startTime, endTime};
        alarmCount++;
    } else {
        // Serial.println("Max number of alarms reached");
    }
}

void TimeManager::checkAlarms(Buzzer &buzzer, Fan &fan, DHTSensor &dht,
                              GasSensor &gas, UVSensor &uv,
                              PresenceSensor &presence) {
    anyAlarmTriggered = false;
    time_t now = currentTime + millis() / 1000;
    struct tm *timeinfo = localtime(&now);
    for (int i = 0; i < alarmCount; i++) {
        if (isCurrentDayInRange(alarms[i].days) &&
            isCurrentTimeInRange(alarms[i].startTime, alarms[i].endTime)) {
            bool trigger = false;
            if (alarms[i].sensor == "TEMP" && alarms[i].condition == "VALUE") {
                if (alarms[i].comparison == "ABOVE" &&
                    dht.getTemperature() > alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "BELOW" &&
                         dht.getTemperature() < alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "EQUAL" &&
                         dht.getTemperature() == alarms[i].value)
                    trigger = true;
            } else if (alarms[i].sensor == "HUM" &&
                       alarms[i].condition == "VALUE") {
                if (alarms[i].comparison == "ABOVE" &&
                    dht.getHumidity() > alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "BELOW" &&
                         dht.getHumidity() < alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "EQUAL" &&
                         dht.getHumidity() == alarms[i].value)
                    trigger = true;
            } else if (alarms[i].sensor == "GAS" &&
                       alarms[i].condition == "VALUE") {
                if (alarms[i].comparison == "ABOVE" &&
                    gas.getGasValue() > alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "BELOW" &&
                         gas.getGasValue() < alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "EQUAL" &&
                         gas.getGasValue() == alarms[i].value)
                    trigger = true;
            } else if (alarms[i].sensor == "GAS" &&
                       alarms[i].condition == "DETECTED" &&
                       gas.isGasDetected()) {
                trigger = true;
            } else if (alarms[i].sensor == "UV" &&
                       alarms[i].condition == "VALUE") {
                if (alarms[i].comparison == "ABOVE" &&
                    uv.getUVValue() > alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "BELOW" &&
                         uv.getUVValue() < alarms[i].value)
                    trigger = true;
                else if (alarms[i].comparison == "EQUAL" &&
                         uv.getUVValue() == alarms[i].value)
                    trigger = true;
            } else if (alarms[i].sensor == "MOTION" &&
                       alarms[i].condition == "DETECTED" &&
                       presence.getPresence()) {
                trigger = true;
            }

            if (trigger) {
                Serial.println("Alarm triggered: " + alarms[i].sensor);
                buzzer.setState(true);
                fan.setState(true);
                anyAlarmTriggered = true;
            }
        }
    }
}

bool TimeManager::isAnyAlarmTriggered() const { return anyAlarmTriggered; }

bool TimeManager::isCurrentTimeInRange(const String &startTime,
                                       const String &endTime) {
    time_t now = currentTime + millis() / 1000;
    struct tm *timeinfo = localtime(&now);
    int currentMinutes = timeinfo->tm_hour * 60 + timeinfo->tm_min;
    int startMinutes = startTime.substring(0, 2).toInt() * 60 +
                       startTime.substring(3, 5).toInt();
    int endMinutes =
        endTime.substring(0, 2).toInt() * 60 + endTime.substring(3, 5).toInt();

    if (startMinutes <= endMinutes) {
        return currentMinutes >= startMinutes && currentMinutes <= endMinutes;
    } else {
        return currentMinutes >= startMinutes || currentMinutes <= endMinutes;
    }
}

bool TimeManager::isCurrentDayInRange(const String &days) {
    time_t now = currentTime + millis() / 1000;
    struct tm *timeinfo = localtime(&now);
    int currentDay = timeinfo->tm_wday;
    return days.indexOf(String(currentDay)) != -1;
}