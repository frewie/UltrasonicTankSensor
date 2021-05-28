#ifndef _ultrasonic_level_H_
#define _ultrasonic_level_H_

#include <sensors/sensor.h>
#include <DS1603L.h>              //https://github.com/wvmarle/Arduino_DS1603L


/**
 * @brief Reads a XKC-DS1603L.V1 ultrasonic
 * sensor and outputs the number of microseconds that it takes the sound
 * wave to bounce off an object and return to the sensor.
 * 
 * Output must be sent to a Linear Transform to convert to a distance.
 * 
 * @see https://github.com/SignalK/SensESP/tree/master/examples/ultrasonic_level_sensor
 * 
 * @param trigger_pin The pin that is set to HIGH/LOW to start/stop the measurement.
 * 
 * @param input_pin The pin that reads the measurement
 * 
 * @param read_delay The time between reads of the sensor, in ms. Defaults to 1000.
 * 
 * @param config_path The path to configure read_delay in the Config UI.
 **/
class UltrasonicLevel : public NumericSensor {
 public:
  UltrasonicLevel(int8_t txPin, int8_t rxPin,
                     uint read_delay = 1000, String config_path = "");
  void enable() override final;

 private:
  int8_t tx_pin_;
  int8_t rx_pin_;
  uint read_delay_;
  DS1603L* ultrasonic_sensor_;

  virtual void get_configuration(JsonObject& doc) override;
  virtual bool set_configuration(const JsonObject& config) override;
  virtual String get_config_schema() override;
  uint getSensorReading ();
};

// FIXME: Uncomment the following once the PIO Xtensa toolchain is updated
// [[deprecated("Use UltrasonicDistance instead.")]]
typedef UltrasonicLevel UltrasonicSens;

#endif
