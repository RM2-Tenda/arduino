#include <ArduinoJson.h> // Include the ArduinoJson library
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char *ssid = "Mario Office";
const char *password = "72519111";
String serverName = "http://192.168.8.114:5000/api/button";
String statisticsAPI = "http://192.168.8.114:5000/api/statistics";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
unsigned long lastButtonCheck = 0;
unsigned long buttonCheckDelay = 2000; // Check button status every 2 seconds

int ledPin = D2; // GPIO pin where the LED is connected

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        if (data.length() > 0) {
            Serial.println(data);
            sendDataToServer(data);
        }
    }

    if (millis() - lastButtonCheck > buttonCheckDelay) {
        checkButtonState();
        lastButtonCheck = millis();
    }
}

void sendDataToServer(String data) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, statisticsAPI + "?data=" + data);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String httpRequestData = "data=" + data;
        int httpResponseCode = http.POST(httpRequestData);

        if (httpResponseCode > 0) {
            String response = http.getString();
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

void checkButtonState() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, serverName);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("Button state: " + payload);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            bool result = doc["result"];
            if (result) {
                Serial.println("It's true!");
                digitalWrite(ledPin, HIGH);
            } else {
                Serial.println("Nope");
                digitalWrite(ledPin, LOW);
            }
        } else {
            Serial.println("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}