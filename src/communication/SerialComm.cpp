#include "SerialComm.h"

void SerialComm::begin() {
    Serial.begin(9600);
    Serial.setTimeout(2000);
}

void SerialComm::sendData(const DHTSensor &dht, const GPSSensor &gps,
                          const PresenceSensor &presence, const GasSensor &gas,
                          const UVSensor &uv) {

    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    bool presenceDetected = presence.getPresence();
    int gasValue = gas.getGasValue();
    bool gasDetected = gas.isGasDetected();
    int uvValue = uv.getUVValue();
    double latitude = gps.getLatitude();
    double longitude = gps.getLongitude();

    Serial.print(humidity);
    Serial.print(",");
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(presenceDetected);
    Serial.print(",");
    Serial.print(gasValue);
    Serial.print(",");
    Serial.print(gasDetected);
    Serial.print(",");
    Serial.print(uvValue);
    Serial.print(",");
    Serial.print(latitude, 6);
    Serial.print(",");
    Serial.println(longitude, 6);
}

void SerialComm::receiveCommands(LEDStrip &ledStrip, Buzzer &buzzer, Fan &fan,
                                 DHTSensor &dht, GasSensor &gas, UVSensor &uv,
                                 TimeManager &timeManager) {
    while (Serial.available() > 0) {
        char command[100];
        int bytesRead = Serial.readBytesUntil('\n', command, sizeof(command) - 1);
        command[bytesRead] = '\0';
        processCommand(command, ledStrip, buzzer, fan, dht, gas, uv, timeManager);
    }
}

void SerialComm::processCommand(const char* command, LEDStrip &ledStrip,
                                Buzzer &buzzer, Fan &fan, DHTSensor &dht,
                                GasSensor &gas, UVSensor &uv,
                                TimeManager &timeManager) {
    // Serial.print(F("Processing command: "));
    // Serial.println(command);

    const char* commaPos = strchr(command, ',');
    if (commaPos == nullptr) {
        // Serial.println(F("Invalid command format"));
        return;
    }

    int commaIndex = commaPos - command;
    char cmdType[20];
    strncpy(cmdType, command, commaIndex);
    cmdType[commaIndex] = '\0';
    const char* params = command + commaIndex + 1;

    if (strcmp(cmdType, "SET_LED_PATTERN") == 0) {
        const char* commaPos2 = strchr(params, ',');
        int commaIndex2 = commaPos2 ? commaPos2 - params : -1;
        uint8_t pattern = atoi(params);
        if (pattern == 9 && commaIndex2 != -1) {
            const char* colorHex = params + commaIndex2 + 1;
            long color = strtol(colorHex, NULL, 16);
            ledStrip.setCustomColor(color);
        } else {
            ledStrip.setPattern(pattern);
        }
        ledStrip.updatePattern();
    } else if (strcmp(cmdType, "SET_BUZZER") == 0) {
        if (strcmp(params, "ON") == 0) {
            buzzer.on();
            buzzer.setCommandState(true);
        } else if (strcmp(params, "OFF") == 0) {
            buzzer.off();
            buzzer.setCommandState(false);
        }
    } else if (strcmp(cmdType, "SET_FAN") == 0) {
        if (strcmp(params, "ON") == 0) {
            fan.on();
            fan.setCommandState(true);
        } else if (strcmp(params, "OFF") == 0) {
            fan.off();
            fan.setCommandState(false);
        }
    } else if (strcmp(cmdType, "SET_DHT_THRESHOLD") == 0) {
        const char* commaPos2 = strchr(params, ',');
        if (commaPos2 == nullptr) {
            // Serial.println(F("Invalid DHT threshold format"));
            return;
        }
        int commaIndex2 = commaPos2 - params;
        float tempThreshold = atof(params);
        float humThreshold = atof(params + commaIndex2 + 1);
        dht.setThresholds(tempThreshold, humThreshold);
    } else if (strcmp(cmdType, "SET_GAS_THRESHOLD") == 0) {
        int gasThreshold = atoi(params);
        gas.setThreshold(gasThreshold);
    } else if (strcmp(cmdType, "SET_UV_THRESHOLD") == 0) {
        int uvThreshold = atoi(params);
        uv.setThreshold(uvThreshold);
    } else if (strcmp(cmdType, "SET_ALARM") == 0) {
        int paramIndex = 0;
        SensorType sensor = static_cast<SensorType>(atoi(getNextParam(params, paramIndex)));
        ConditionType condition = static_cast<ConditionType>(atoi(getNextParam(params, paramIndex)));
        ComparisonType comparison = static_cast<ComparisonType>(atoi(getNextParam(params, paramIndex)));
        float value = atof(getNextParam(params, paramIndex));
        uint8_t days = atoi(getNextParam(params, paramIndex));
        uint16_t startTime = atoi(getNextParam(params, paramIndex));
        uint16_t endTime = atoi(getNextParam(params, paramIndex));

        timeManager.setAlarm(sensor, condition, comparison, value, days, startTime, endTime);
    } else if (strcmp(cmdType, "SET_TIME") == 0) {
        timeManager.setTime(params);
    }
}

const char* SerialComm::getNextParam(const char* params, int &paramIndex) {
    const char* nextComma = strchr(params + paramIndex, ',');
    int length = nextComma ? nextComma - (params + paramIndex) : strlen(params + paramIndex);
    static char param[20];
    strncpy(param, params + paramIndex, length);
    param[length] = '\0';
    paramIndex += length + 1;
    return param;
}