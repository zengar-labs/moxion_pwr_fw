#pragma once

#include "Drivers.h"
/**
 * @brief Example FanController base
 */
class FanController {
public:
  FanController(VoltageSensorInterface& temp_sensor_raw, /*voltage in raw val*/
                GpioOutputInterface& sensor_power_enable,/*pwr spply to Sensor*/
                GpioOutputInterface& fan_relay_enable,   /*Relay Ctrl to fan*/
                PwmOutputInterface& fan_output_spd)      /* Fan Speed Ctl */
    : _temp_sensor_raw(temp_sensor_raw),
      _sensor_power_enable(sensor_power_enable),
      _fan_relay_enable(fan_relay_enable),
      _fan_output_spd(fan_output_spd),
      _fan_enabled(false) {}

  /*
   * @brief One time executed initialization method
   */
  void init() {
    // run some initialization code here ex diagnostigs for the relays, etc.
  };

  /**
   * @brief Periodic loop method that will be called every 500ms
   */
  void loop() {
    // Enable sensor power
    _sensor_power_enable.setOutput(true);
    
    // Req 1. Wait for 5 seconds for sensor stabilization
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Read temperature sensor voltage (in raw val)
    float voltage = _temp_sensor_raw.getVoltage();

    if ((voltage < SensorMinVol) || (voltage > SensorSplyVol)) {
      // SATURATE to MIX or MAX
      if (voltage < SensorMinVol)
        voltage = SensorMinVol;
      else
        voltage = SensorSplyVol;
      // TODO Call some fault handler maybe or diag
    }

    // Calculate temperature using the transfer function
    float temperature = (voltage / SensorSplyVol) * (SensorCoeffA + (SensorCoeffB / 1000.0f) * voltage);

  // Req 2. Implement a fan controller that will control the fan based on the temperature

    // Check temperature conditions and control the fan
    if (temperature > FanOnTempThreshold && !_fan_enabled) {
      // Enable the fan relay
      _fan_relay_enable.setOutput(true);
      _fan_enabled = true; // Update the fan enabled flag
    } else if (temperature < FanOffTempThreshold && _fan_enabled) {
      // Disable the fan relay
      _fan_relay_enable.setOutput(false);
      _fan_enabled = false;
    }

    if (_fan_enabled) {// If fan controller is on then control the speed
      // Calculate fan PWM duty cycle based on temperature
      float pwmDutyCycle = calculateFanDutyCycle(temperature);

      // Set the fan PWM output
      _fan_output_spd.setOutputDuty(pwmDutyCycle);
    }
  }

private:
  VoltageSensorInterface& _temp_sensor_raw;
  GpioOutputInterface&    _sensor_power_enable;
  GpioOutputInterface&    _fan_relay_enable;
  PwmOutputInterface&     _fan_output_spd;
  bool _fan_enabled; // To know the status of the fan (relay)

  static constexpr float SensorMinVol = 0.25f;
  static constexpr float SensorSplyVol = 5.0f;
  static constexpr float SensorCoeffA = 1.375f;
  static constexpr float SensorCoeffB = 22.5f;

  static constexpr float FanOnTempThreshold = 60.0f;
  static constexpr float FanOffTempThreshold = 50.0f;
/**
 * @Transfer Function
  VOUT = (V+/5 V) × [1.375 V +(22.5 mV/°C) × TA] 

| Temperature (t) | PWM Output = f(t)    |
|-----------------|----------------------|
| 50-99           | = (t-50) / 2         |
| 100-129         | = (t-98) * 2 + 20    |
| 130-140         | = 90                 |
| > 140           | = 100                |
 */

  float calculateFanDutyCycle(float temperature) {
    if (temperature >= 50.0f && temperature < 100.0f) {
      return (temperature - 50.0f) / 2.0f;
    } else if (temperature >= 100.0f && temperature < 130.0f) {
      return (temperature - 98.0f) * 2.0f + 20.0f;
    } else if (temperature >= 130.0f && temperature <= 140.0f) {
      return 90.0f;
    } else if (temperature > 140.0f) {
      return 100.0f;
    } else {
      // Default duty for anything belowr 50c
      return 0.0f;
    }
  }
};