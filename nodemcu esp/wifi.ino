#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <time.h>

const char *ssid = "miPhone";
const char *password = "12345678";
String serverName = "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/button";
String statisticsAPI =
    "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/statistics";
String commandAPI =
    "http://rm2-backend-f78fbf915aa5.herokuapp.com/api/commands";
String deviceID = "device_001";

unsigned long lastButtonCheck = 0;
unsigned long buttonCheckDelay = 1500;
unsigned long lastTimeSync = 0;
unsigned long timeSyncDelay = 60000;

int ledPin = D2;

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        // Serial.print(".");
    }
    // Serial.println("WiFi connected.");
    // Serial.println("IP address: ");
    // Serial.println(WiFi.localIP());

    // configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    // syncTimeWithArduino();
}

void loop() {
    if (millis() - lastTimeSync > buttonCheckDelay) {
        if (Serial.available()) {
            String data = Serial.readStringUntil('\n');
            parseAndSendData(data);
        }
    }

    if (millis() - lastTimeSync > timeSyncDelay) {
        syncTimeWithArduino();
        lastTimeSync = millis();
    }

    checkForCommands();
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
            // Serial.println(response);
        } else {
            // Serial.print("Error code: ");
            //  Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        // Serial.println("WiFi Disconnected");
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
            // Serial.println("Received command: " + payload);
            sendCommandToSerial(payload);
        } else {
            // Serial.println("Error code: ");
            //  Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        // Serial.println("WiFi Disconnected");
    }
}

void sendCommandToSerial(String command) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, command);
    String cmd = doc["command"];
    Serial.print(cmd);
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