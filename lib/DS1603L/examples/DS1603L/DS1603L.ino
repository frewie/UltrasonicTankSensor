#include <DS1603L.h>

// Change this to reflect how your sensor is connected.
#include <SoftwareSerial.h>
const byte txPin = 4;                               // tx of the Arduino to rx of the sensor
const byte rxPin = 5;                               // rx of the Arduino to tx of the sensor
SoftwareSerial sensorSerial(rxPin, txPin);

// If your sensor is connected to Serial, Serial1, Serial2, AltSoftSerial, etc. pass that object to the sensor constructor.
DS1603L sensor(sensorSerial);

void setup() {
  Serial.begin(115200);                             // Here the sensor output is printed.
  sensorSerial.begin(9600);                         // Sensor transmits its data at 9600 bps.
  sensor.begin();                                   // Initialise the sensor library.
  Serial.println(F("Setup done."));
}

void loop() {
  Serial.println(F("Starting reading."));
  unsigned int reading = sensor.readSensor();       // Call this as often or as little as you want - the sensor transmits every 1-2 seconds.
  byte sensorStatus = sensor.getStatus();           // Check the status of the sensor (not detected; checksum failed; reading success).
  switch (sensorStatus) {                           // For possible values see DS1603L.h
    case DS1603L_NO_SENSOR_DETECTED:                // No sensor detected: no valid transmission received for >10 seconds.
      Serial.println(F("No sensor detected (yet). If no sensor after 1 second, check whether your connections are good."));
      break;

    case DS1603L_READING_CHECKSUM_FAIL:             // Checksum of the latest transmission failed.
      Serial.print(F("Data received; checksum failed. Latest level reading: "));
      Serial.print(reading);
      Serial.println(F(" mm."));
      break;

    case DS1603L_READING_SUCCESS:                   // Latest reading was valid and received successfully.
      Serial.print(F("Reading success. Water level: "));
      Serial.print(reading);
      Serial.println(F(" mm."));
      break;
  }
  delay(2000);
}
