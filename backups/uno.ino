#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define PRESENCE_PIN 3

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx and Presence Sensor Test");

  dht.begin();

  pinMode(PRESENCE_PIN, INPUT);
}

void loop() {
  delay(2000);

  int presence = digitalRead(PRESENCE_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print(h);
  Serial.print(",");
  Serial.print(t);
  Serial.print(",");
  Serial.println(presence);
}