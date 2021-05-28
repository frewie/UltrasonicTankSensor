/*
 * DS1602L library.
 * 
 * Written by Wouter van Marle, wouter@cityhydronics.hk, 2018
 */

#ifndef __DS1603L__
#define __DS1603L__

#include <Stream.h>                                             // Generic Stream library, upon which a.o. Serial and SoftwareSerial are built.

#define DS1603L_NO_SENSOR_DETECTED 0                            // Sensor not detected.
#define DS1603L_READING_SUCCESS 1                               // Latest reading successful.
#define DS1603L_READING_CHECKSUM_FAIL 2                         // Latest reading failed checksum.

class DS1603L {
  public:
    DS1603L(Stream* stream);                                    // Constructor.
    void begin();                                               // Initialisation.
    uint16_t readSensor();                                      // Get the latest data from the Serial buffer and process it.
    uint8_t getStatus();                                        // Return the status of the sensor.


  private:
    Stream *sensorSerial;                                       // The sensor connection - a Stream object.
    uint8_t sensorStatus;                                       // The current status of the sensor.
    uint32_t serialData;                                        // A 4-byte buffer to store the latest transmission data.
    uint16_t reading;                                           // The latest reading as returned by the sensor: water level in mm.
    uint32_t lastReadingTime;                                   // When the last successful reading was received, to detect disconnection.
};
#endif
