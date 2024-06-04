#ifndef GPSSENSOR_H
#define GPSSENSOR_H

#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

class GPSSensor {
  public:
    GPSSensor(uint8_t txPin);
    void begin();
    void read();
    double getLatitude() const;
    double getLongitude() const;

  private:
    SoftwareSerial gpsSerial;
    TinyGPSPlus gps;
    double latitude;
    double longitude;
};

#endif