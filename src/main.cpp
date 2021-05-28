#include <Arduino.h>

#include "sensesp_app.h"
#include "signalk/signalk_output.h"
#include "transforms/linear.h"
#include "transforms/moving_average.h"
#include <sensesp_app_builder.h>
#include "ultrasonic_level.h"

#define TX_PIN D4
#define RX_PIN D3

//#define SERIAL_DEBUG_DISABLED


// SensESP builds upon the ReactESP framework. Every ReactESP application
// defines an "app" object vs defining a "main()" method.
ReactESP app([]() {

// Some initialization boilerplate when in debug mode...
#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif



  // Create the global SensESPApp() object.
  // sensesp_app = new SensESPApp();

  SensESPAppBuilder builder;


  sensesp_app = builder.set_standard_sensors(ALL)
            ->set_hostname("freshwaterport")
            ->set_sk_server("10.10.10.1", 3000) // Don't specify server address or port
            ->set_wifi("SSID", "Password")
            ->get_app(); 

  // The "Signal K path" identifies this sensor to the Signal K server. Leaving
  // this blank would indicate this particular sensor (or transform) does not
  // broadcast Signal K data.
  // To find valid Signal K Paths that fits your need you look at this link:
  // https://signalk.org/specification/1.4.0/doc/vesselsBranch.html
  //const char *sk_path = "tanks.freshWater.port.currentLevel";
  //const char *sk_path = "tanks.freshWater.starbord.currentLevel";
  const char *sk_path = "tanks.fuel.daytank.currentLevel";


  // The "Configuration path" is combined with "/config" to formulate a URL
  // used by the RESTful API for retrieving or setting configuration data.
  // It is ALSO used to specify a path to the SPIFFS file system
  // where configuration data is saved on the MCU board. It should
  // ALWAYS start with a forward slash if specified. If left blank,
  // that indicates this sensor or transform does not have any
  // configuration to save, or that you're not interested in doing
  // run-time configuration.

  const char *ultrasonic_in_config_path =
      "/freshWaterTank_starboard/ultrasonic_in";
  const char *linear_config_path = "/freshWaterTank_starboard/linear";
  const char *ultrasonic_ave_samples = "/freshWaterTank_starboard/samples";

  // Create a sensor that is the source of our data, that will be read every
  // readDelay ms. It is an ultrasonic distance sensor that sends out an
  // acoustical pulse in response to a 100 micro-sec trigger pulse from the ESP.
  // The return acoustical pulse width can be converted to a distance by the
  // formula 2*distance = pulse_width/speed_of_sound With pulse_width in
  // micro-sec and distance in cm, 2*speed_of_sound = 58 The sensor is mounted
  // at the top of a water tank that is 25 cm deep.
  uint read_delay = 1000;

  auto *ultrasonic_sensor = new UltrasonicLevel(
      TX_PIN, RX_PIN, read_delay, ultrasonic_in_config_path);



// see: https://github.com/SignalK/SensESP/wiki/Using-AnalogInput-to-Calculate-a-Tank-Level

// without offset 
//const float = empty_value = 0
// full_value = 200 
// range = full_value - empty_value = 200
// divisor = range / 100 (200 / 100 = 2)
// multiplier = 1 / divisor (1 / 2 = 0.5)


// with offset 

  const float empty_value = 0; // in mm 
  const float full_value = 200; // in mm  
  const float range = full_value - empty_value;
  const float divisor = range / 100.0;
  const float multiplier = 1.0 / divisor; //  (1 / 4.5 = 0.0222222)
  const float offset = 100.0 - full_value * multiplier; // (100 - (500 x 0.0222222) = 38.8889)

// Now, you add Linear to your main.cpp with your calculated multiplier and offset values:

  float scale = 1.0;

  // Wire up the output of the analog input to the Linear transform,
  // and then output the results to the Signal K server.
  ultrasonic_sensor
      ->connect_to(new Linear(multiplier, offset, linear_config_path))
      ->connect_to(new MovingAverage(10, scale, ultrasonic_ave_samples))
      ->connect_to(new SKOutputNumber(sk_path));

  // Start the SensESP application running
  sensesp_app->enable();
});