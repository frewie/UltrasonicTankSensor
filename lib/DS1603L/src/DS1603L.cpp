#include "DS1603L.h"
#include <Arduino.h>                                            // To have access to millis().

DS1603L::DS1603L(Stream* stream) {                              // General constructor.
  sensorSerial = stream;
}

void DS1603L::begin() {                                         // Initialisation of some variables.
  sensorStatus = DS1603L_NO_SENSOR_DETECTED;
  lastReadingTime = 0;
  serialData = 0;
}

uint16_t DS1603L::readSensor() {
  while (sensorSerial->available()) {                           // There is data in the buffer, process it.
    uint8_t serialByte = sensorSerial->read();                  // Read a byte.
    serialData = (serialData << 8) | serialByte;                // Use an unsigned long as convenient 4-byte rolling buffer.

    // Check whether we have a complete set of data: this should be the case if the high byte = 255.
    // The sensor returns four bytes at a time, which are stored in the serialData variable. Every time a new byte arrives this buffer
    // serialData is left shifted, old bytes just drop out, and only the latest four are kept. A complete transmission now looks like
    // this in serialData:
    // Byte 3: Start byte (255).
    // Byte 2: Data_H (reading in mm, high byte).
    // Byte 1: Data_L (reading in mm, low byte).
    // Byte 0: Checksum.
    //
    // In the calculation of checksum we don't have to worry about higher bits, only the low 8 bits are actually used thanks to
    // checksum being declared as 8-bit integer.
    //    
    if (serialData >> 24 == 255) {                              // Byte 3 is a start byte.
      uint8_t checksum = 255;                                   // Header
      checksum += (serialData >> 16);                           // + data_H
      checksum += (serialData >> 8);                            // + data_L
      if (checksum == (serialData & 0xFF)) {                    // Check whether the checksum is correct.
        reading = serialData >> 8 & 0xFFFF;                     // Byte 2 = data_H, byte 1 = data_L. Together they're the level in mm.
        sensorStatus = DS1603L_READING_SUCCESS;                 // Successful reading!
        lastReadingTime = millis();                             // Check when we had the latest successful reading. Sensor should transmit every 1-2 seconds.
      }
      else {
        sensorStatus = DS1603L_READING_CHECKSUM_FAIL;           // Failed the checksum; returning previous reading.
      }
    }
  }
  if (millis() - lastReadingTime > 10000) {                     // Sensor should transmit every 1-2 seconds.
    reading = 0;
    sensorStatus = DS1603L_NO_SENSOR_DETECTED;                  // Sensor disconnected or so? 
  }
  return reading;
}

uint8_t DS1603L::getStatus() {
  return sensorStatus;
}

