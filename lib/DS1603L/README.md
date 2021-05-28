# Arduino_DS1603L
Arduino library for the DS1603L ultrasound sensor.

Tested on Arduino Pro Mini, expected to work on any other processor that offers a Serial type interface.

How to use this library
-

Download the complete library (click the green button, click Download ZIP), then unpack the library files in your ~/Arduino/libraries folder, where all your user libraries live.
In the ./examples directory you will find a simple test sketch, that reads the sensor and displays the result on the Serial monitor. Make sure the pin definitions in the script match the connections of your sensor, and that the speed the Serial interface is set matches that of your Serial monitor.

The sensor may be connected to a hardware Serial port, SoftwareSerial, AltSoftSerial, or any other connection that offers the Stream interface. In your sketch first define this connection, then pass that to the constructor of the library. Then in setup() call the .begin() function, and in loop() call .readSensor() as often or as little as you like. The sensor transmits it's latest reading every 1-2 seconds, attempting to read it more frequently than this will simply return the latest measurement.


Library functions
-

    DS1603L(Stream &stream)
The general constructor. stream is a Stream object (e.g. SoftwareSerial).

    void begin()
Call this from setup(). Initialises the library.

    uint16_t readSensor()
Read the latest value of the sensor, returns the latest reading as waterlevel in mm. If no sensor is connected, a value of 0 is returned. The sensor has an effective range of 50-2000 mm.

    uint8_t getStatus();
Get the status of the readings; the possible return values are defined in DS1603L.h
