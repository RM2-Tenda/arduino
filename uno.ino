#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define PRESENCE_PIN 3
#define GAS_SENSOR_AO A0
#define GAS_SENSOR_DO 4
#define UV_SENSOR_PIN A1
#define FAN_1_PIN 5
#define BUZZER_PIN 7

void setup() {
    Serial.begin(9600);
    Serial.println("DHTxx, Presence Sensor, Gas Sensor, and UV Sensor Test");

    dht.begin();

    pinMode(PRESENCE_PIN, INPUT);
    pinMode(GAS_SENSOR_DO, INPUT);
    pinMode(FAN_1_PIN, OUTPUT);
    pinMode(FAN_2_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    delay(2000);

    int presence = digitalRead(PRESENCE_PIN);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int gasValue = analogRead(GAS_SENSOR_AO);
    bool gasDetected = !digitalRead(GAS_SENSOR_DO);
    int uvValue = analogRead(UV_SENSOR_PIN);
    int uvValueIndex = 0.05 * uvValue - 1;

    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print(h);
    Serial.print(",");
    Serial.print(t);
    Serial.print(",");
    Serial.print(presence);
    Serial.print(",");
    Serial.print(gasValue);
    Serial.print(",");
    Serial.print(gasDetected);
    Serial.print(",");
    Serial.println(uvValue);
    delay(100);

    if (gasDetected) {
        digitalWrite(FAN_1_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
    } else {
        digitalWrite(FAN_1_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }
}