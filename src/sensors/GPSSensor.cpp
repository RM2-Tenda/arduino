#include "GPSSensor.h"

GPSSensor::GPSSensor(uint8_t txPin)
    : gpsSerial(-1, txPin), latitude(NAN), longitude(NAN) {}

void GPSSensor::begin() { gpsSerial.begin(9600); }

void GPSSensor::read() {
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        gps.encode(c);
    }

    if (gps.location.isUpdated()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
}

double GPSSensor::getLatitude() const { return latitude; }

double GPSSensor::getLongitude() const { return longitude; }