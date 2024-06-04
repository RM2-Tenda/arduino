#include "SerialComm.h"

void SerialComm::begin() {
    Serial.begin(9600);
    Serial.setTimeout(2000); // Increase timeout to ensure full command is read
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
        String command = Serial.readStringUntil('\n');
        processCommand(command, ledStrip, buzzer, fan, dht, gas, uv,
                       timeManager);
    }
}

void SerialComm::processCommand(const String &command, LEDStrip &ledStrip,
                                Buzzer &buzzer, Fan &fan, DHTSensor &dht,
                                GasSensor &gas, UVSensor &uv,
                                TimeManager &timeManager) {
    int commaIndex = command.indexOf(',');
    String cmdType = command.substring(0, commaIndex);
    String params = command.substring(commaIndex + 1);

    if (cmdType == "SET_LED_PATTERN") {
        int commaIndex2 = params.indexOf(',');
        uint8_t pattern = params.substring(0, commaIndex2).toInt();
        if (pattern == 9) {
            String colorHex = params.substring(commaIndex2 + 1);
            long color = strtol(colorHex.c_str(), NULL, 16);
            ledStrip.setCustomColor(color);
        } else {
            ledStrip.setPattern(pattern);
        }
        ledStrip.updatePattern();
    } else if (cmdType == "SET_BUZZER") {
        if (params == "ON") {
            buzzer.on();
        } else if (params == "OFF") {
            buzzer.off();
        }
    } else if (cmdType == "SET_FAN") {
        if (params == "ON") {
            fan.on();
        } else if (params == "OFF") {
            fan.off();
        }
    } else if (cmdType == "SET_DHT_THRESHOLD") {
        int commaIndex2 = params.indexOf(',');
        float tempThreshold = params.substring(0, commaIndex2).toFloat();
        float humThreshold = params.substring(commaIndex2 + 1).toFloat();
        dht.setThresholds(tempThreshold, humThreshold);
    } else if (cmdType == "SET_GAS_THRESHOLD") {
        int gasThreshold = params.toInt();
        gas.setThreshold(gasThreshold);
    } else if (cmdType == "SET_UV_THRESHOLD") {
        int uvThreshold = params.toInt();
        uv.setThreshold(uvThreshold);
    } else if (cmdType == "SET_ALARM") {
        // Parse additional fields for comparison and value
        int commaIndex2 = params.indexOf(',');
        String sensor = params.substring(0, commaIndex2);
        params = params.substring(commaIndex2 + 1);

        commaIndex2 = params.indexOf(',');
        String condition = params.substring(0, commaIndex2);
        params = params.substring(commaIndex2 + 1);

        commaIndex2 = params.indexOf(',');
        String comparison = params.substring(0, commaIndex2);
        params = params.substring(commaIndex2 + 1);

        commaIndex2 = params.indexOf(',');
        float value = params.substring(0, commaIndex2).toFloat();
        params = params.substring(commaIndex2 + 1);

        commaIndex2 = params.indexOf(',');
        String days = params.substring(0, commaIndex2);
        params = params.substring(commaIndex2 + 1);

        commaIndex2 = params.indexOf(',');
        String startTime = params.substring(0, commaIndex2);
        String endTime = params.substring(commaIndex2 + 1);

        timeManager.setAlarm(sensor, condition, comparison, value, days,
                             startTime, endTime);
    }
}