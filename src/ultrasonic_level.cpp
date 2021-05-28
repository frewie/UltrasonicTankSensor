#include "ultrasonic_level.h"
#include "Arduino.h"
#include "sensesp.h"
#include <DS1603L.h>              //https://github.com/wvmarle/Arduino_DS1603L
#include <SoftwareSerial.h>


UltrasonicLevel::UltrasonicLevel(int8_t txPin, int8_t rxPin,
                                       uint read_delay, String config_path)
    : NumericSensor(config_path),
      tx_pin_{txPin},
      rx_pin_{rxPin},
      read_delay_{read_delay} {

     load_configuration();
}



void UltrasonicLevel::enable() {
  SoftwareSerial* serial = new SoftwareSerial(tx_pin_, rx_pin_);
  
  ultrasonic_sensor_ = new DS1603L(serial);

  serial->begin(38400, SWSERIAL_8N1);
  serial->begin(9600);                         // Sensor transmits its data at 9600 bps.
  ultrasonic_sensor_->begin();  // Initialise the sensor library.

  app.onRepeat(read_delay_, [this]() {
    long last_time = micros();
    while (micros() - last_time < 100) {
      yield();
    }

    this->emit(this->getSensorReading());  // 50 microsecond timeout
  });
}

void UltrasonicLevel::get_configuration(JsonObject& root) {
  root["read_delay"] = read_delay_;
};

static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "Number of milliseconds between each thermocouple read " }
    }
  })###";

String UltrasonicLevel::get_config_schema() { return FPSTR(SCHEMA); }

bool UltrasonicLevel::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay_ = config["read_delay"];
  return true;
}

uint UltrasonicLevel::getSensorReading () {
    unsigned int reading;
  
    reading = ultrasonic_sensor_->readSensor();       // Call this as often or as little as you want - the sensor transmits every 1-2 seconds.
    byte sensorStatus = ultrasonic_sensor_->getStatus();           // Check the status of the sensor (not detected; checksum failed; reading success).
    switch (sensorStatus) {                           // For possible values see DS1603L.h
      case DS1603L_NO_SENSOR_DETECTED:                // No sensor detected: no valid transmission received for >10 seconds.
        Serial.println(F("No sensor detected (yet). If no sensor after 1 second, check whether your connections are good."));
        break;

      case DS1603L_READING_SUCCESS:                   // Latest reading was valid and received successfully.
        Serial.println(F("Reading success."));
        Serial.println(reading);
        Serial.println(F(" mm."));
        break;

      case DS1603L_READING_CHECKSUM_FAIL:             // Checksum of the latest transmission failed.
        Serial.print(F("Data received; checksum failed. Latest level reading: "));
        break;

    }

     return reading;
}
