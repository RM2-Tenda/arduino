#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <map>

const char *ssid = "miPhone";
const char *password = "12345678";
String serverName = "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/button";
String statisticsAPI = "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/statistics";
String commandAPI = "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/commands";
String clearCommandAPI = "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/commands/clear";
String alarmsAPI = "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms";
String deviceID = "device_001";

unsigned long lastButtonCheck = 0;
unsigned long buttonCheckDelay = 2000;
unsigned long lastTimeSync = 0;
unsigned long timeSyncDelay = 60000;
unsigned long lastCommandCheck = 0;
unsigned long commandCheckDelay = 1000;
unsigned long lastAlarmCheck = 0;
unsigned long alarmCheckDelay = 1200;

int ledPin = D2;

struct Alarm {
    int id;
    String sensor;
    String condition;
    String comparison;
    float value;
    uint8_t daysBitmask;
    uint16_t startMinutes;
    uint16_t endMinutes;
};

std::map<String, int> sensorTypeMapping = {
    {"TEMP", 0},
    {"HUM", 1},
    {"GAS", 2},
    {"UV", 3},
    {"MOTION", 4}
};

std::map<String, int> conditionTypeMapping = {
    {"VALUE", 0},
    {"DETECTED", 1}
};

std::map<String, int> comparisonTypeMapping = {
    {"ABOVE", 0},
    {"BELOW", 1},
    {"EQUAL", 2}
};

std::vector<Alarm> activeAlarms;

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    syncTimeWithArduino();
}

void loop() {
    if (millis() - lastButtonCheck > buttonCheckDelay) {
        if (Serial.available()) {
            String data = Serial.readStringUntil('\n');
            parseAndSendData(data);
        }
        lastButtonCheck = millis();
    }

    if (millis() - lastTimeSync > timeSyncDelay) {
        syncTimeWithArduino();
        lastTimeSync = millis();
    }

    if (millis() - lastCommandCheck > commandCheckDelay) {
        checkForCommands();
        lastCommandCheck = millis();
    }

    if (millis() - lastAlarmCheck > alarmCheckDelay) {
        checkForAlarms();
        lastAlarmCheck = millis();
    }
}

void parseAndSendData(String data) {
    float humidity, temperature;
    bool presenceValue;
    int gasValue, uvValue;
    bool gasDetected;
    double latitude, longitude;

    int commaIndex = data.indexOf(',');
    humidity = data.substring(0, commaIndex).toFloat();
    data = data.substring(commaIndex + 1);

    commaIndex = data.indexOf(',');
    temperature = data.substring(0, commaIndex).toFloat();
    data = data.substring(commaIndex + 1);

    commaIndex = data.indexOf(',');
    presenceValue = data.substring(0, commaIndex).toInt();
    data = data.substring(commaIndex + 1);

    commaIndex = data.indexOf(',');
    gasValue = data.substring(0, commaIndex).toInt();
    data = data.substring(commaIndex + 1);

    commaIndex = data.indexOf(',');
    gasDetected = data.substring(0, commaIndex).toInt();
    data = data.substring(commaIndex + 1);

    commaIndex = data.indexOf(',');
    uvValue = data.substring(0, commaIndex).toInt();
    data = data.substring(commaIndex + 1);

    commaIndex = data.indexOf(',');
    latitude = data.substring(0, commaIndex).toDouble();
    data = data.substring(commaIndex + 1);

    longitude = data.toDouble();

    sendDataToServer(humidity, temperature, presenceValue, gasValue,
                     gasDetected, uvValue, latitude, longitude);
}

void sendDataToServer(float humidity, float temperature, bool presenceValue,
                      int gasValue, bool gasDetected, int uvValue,
                      double latitude, double longitude) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, statisticsAPI);
        http.addHeader("Content-Type", "application/json");

        DynamicJsonDocument doc(1024);
        doc["device_id"] = deviceID;
        doc["humidity"] = humidity;
        doc["temperature"] = temperature;
        doc["presence"] = presenceValue;
        doc["gas_value"] = gasValue;
        doc["gas_detected"] = gasDetected;
        doc["uv_value"] = uvValue;
        doc["latitude"] = latitude;
        doc["longitude"] = longitude;
        String jsonData;
        serializeJson(doc, jsonData);

        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.print("Data sent: ");
            response.trim();
            Serial.println(response);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void checkForCommands() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, commandAPI + "?device_id=" + deviceID);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String payload = http.getString();
            if (payload.length() > 0) {
                processCommands(payload);
                clearCommands();
            } else {
                Serial.println("No commands received.");
            }
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void processCommands(String command) {
    if (command.length() == 0) {
        return; // Skip processing if the command is empty
    }

    DynamicJsonDocument doc(512);
    deserializeJson(doc, command);
    String cmd = doc["command"];
    if (cmd.length() > 0) {
      Serial.println(cmd);
    }
}

void clearCommands() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, clearCommandAPI);
        http.addHeader("Content-Type", "application/json");

        DynamicJsonDocument doc(1024);
        doc["device_id"] = deviceID;
        String jsonData;
        serializeJson(doc, jsonData);

        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            // Serial.println(response);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void checkForAlarms() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, alarmsAPI + "?device_id=" + deviceID);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String payload = http.getString();
            if (payload.length() > 0) {
                processAlarms(payload);
            } else {
                Serial.println("No alarms received.");
            }
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void processAlarms(String alarms) {
    if (alarms.length() == 0) {
        Serial.println("No alarms received.");
        return;
    }
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, alarms);
    JsonArray alarmsArray = doc["alarms"].as<JsonArray>();

    std::vector<int> receivedAlarmIds;
    for (JsonObject alarm : alarmsArray) {
        int id = alarm["id"];
        String sensor = alarm["sensor"];
        String condition = alarm["condition"];
        String comparison = alarm["comparison"];
        float value = alarm["value"];
        String days = alarm["days"];
        String startTime = alarm["start_time"];
        String endTime = alarm["end_time"];

        // Convert days to bitmask
        uint8_t daysBitmask = 0;
        if (days.indexOf("Mon") != -1) daysBitmask |= (1 << 1);
        if (days.indexOf("Tue") != -1) daysBitmask |= (1 << 2);
        if (days.indexOf("Wed") != -1) daysBitmask |= (1 << 3);
        if (days.indexOf("Thu") != -1) daysBitmask |= (1 << 4);
        if (days.indexOf("Fri") != -1) daysBitmask |= (1 << 5);
        if (days.indexOf("Sat") != -1) daysBitmask |= (1 << 6);
        if (days.indexOf("Sun") != -1) daysBitmask |= (1 << 0);

        // Convert time to minutes since midnight
        uint16_t startMinutes = convertTimeToMinutes(startTime);
        uint16_t endMinutes = convertTimeToMinutes(endTime);

        Alarm newAlarm = {id, sensor, condition, comparison, value, daysBitmask, startMinutes, endMinutes};
        receivedAlarmIds.push_back(id);

        // Check if the alarm is already active
        bool isActive = false;
        for (const auto& activeAlarm : activeAlarms) {
            if (activeAlarm.id == id) {
                isActive = true;
                break;
            }
        }
        // If the alarm is not active, add it
        if (!isActive) {
            activeAlarms.push_back(newAlarm);
            String alarmCommand = "SET_ALARM," + String(sensorTypeMapping[sensor]) + "," +
                              String(conditionTypeMapping[condition]) + "," +
                              String(comparisonTypeMapping[comparison]) + "," +
                              String(value) + "," + String(daysBitmask) + "," +
                              String(startMinutes) + "," + String(endMinutes);

            Serial.println(alarmCommand);
        }
    }

    // Check for alarms that need to be deleted
    for (auto it = activeAlarms.begin(); it != activeAlarms.end();) {
        if (std::find(receivedAlarmIds.begin(), receivedAlarmIds.end(), it->id) == receivedAlarmIds.end()) {
            // Alarm is no longer present, delete it
            String deleteCommand = "DELETE_ALARM," + String(it->id);
            Serial.println(deleteCommand);
            it = activeAlarms.erase(it);
        } else {
            ++it;
        }
    }
}

uint16_t convertTimeToMinutes(String time) {
    int colonIndex = time.indexOf(':');
    int hours = time.substring(0, colonIndex).toInt();
    int minutes = time.substring(colonIndex + 1).toInt();
    return hours * 60 + minutes;
}

void syncTimeWithArduino() {
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    if (timeinfo) {
        char timeString[20];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
        Serial.print("TIME_SYNC ");
        Serial.println(timeString);
    }
}
