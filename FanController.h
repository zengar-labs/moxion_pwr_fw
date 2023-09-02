#pragma once

#include "Drivers.h"
#include "TemperatureSensor.h"
/**
 * @brief Example FanController base
 */
class FanController {
public:
  FanController(TemperatureSensor& temperatureSensor,    /*Temperature from Inverter*/
                GpioOutputInterface& sensor_power_enable,/*pwr spply to Sensor*/
                GpioOutputInterface& fan_relay_enable,   /*Relay Ctrl to fan*/
                PwmOutputInterface& fan_output_spd)      /* Fan Speed Ctl */
    : _temperature_inverter(temperatureSensor),
      _sensor_power_enable(sensor_power_enable),
      _fan_relay_enable(fan_relay_enable),
      _fan_output_spd(fan_output_spd),
      _fan_enabled(false) {}

  /*
   * @brief One time executed initialization method
   */
  void init() {
    // run some initialization code here ex diagnostigs for the relays, etc.
    // Enable sensor power
    _sensor_power_enable.setOutput(true);
    
    // Req 1. Wait for 5 seconds for sensor stabilization
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Todo diag on relays.
  };

  /**
   * @brief Periodic loop method that will be called every 500ms
   */
  void loop() {
    
    // Read temperature sensor from Inverter (Inputs)
    float temperature = _temperature_inverter.getTemperature();

    // Req 2 and 3 Implement a fan controller that will control the fan relay based on the temperature (Outputs)
    fanRelayControl(temperature);

    if (_fan_enabled) {
      // Calculate fan PWM duty cycle based on temperature
      float pwmDutyCycle = calculateFanDutyCycle(temperature);

      // Set the fan PWM output
      _fan_output_spd.setOutputDuty(pwmDutyCycle);
    }
  }

private:
  TemperatureSensor&   _temperature_inverter;
  GpioOutputInterface& _sensor_power_enable;
  GpioOutputInterface& _fan_relay_enable;
  PwmOutputInterface&  _fan_output_spd;
  bool _fan_enabled; // To know the status of the fan (relay)

  static constexpr float FanOnTempThreshold{60.0f};
  static constexpr float FanOffTempThreshold{50.0f};

  static constexpr float FanDCTempThreshold_off {50.0f};
  static constexpr float FanDCTempThreshold_25dc{100.0f};
  static constexpr float FanDCTempThreshold_85dc{130.0f};
  static constexpr float FanDCTempThreshold_90dc{140.0f};
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
    if (temperature >= FanDCTempThreshold_off && temperature < FanDCTempThreshold_25dc) {
      return (temperature - 50.0f) / 2.0f; //0-25%
    } else if (temperature >= FanDCTempThreshold_25dc && temperature < FanDCTempThreshold_85dc) {
      return (temperature - 98.0f) * 2.0f + 20.0f; //24~84%
    } else if (temperature >= FanDCTempThreshold_85dc && temperature <= FanDCTempThreshold_90dc) {
      return 90.0f;
    } else if (temperature > FanDCTempThreshold_90dc) {
      return 100.0f;
    } else {
      // Default duty for anything below 50c
      return 0.0f;
    }
  }

  void fanRelayControl(float temperature) {
    //Req.2 The controller shall enable the fan control relay when the temperature measurement is above 60C.
    if (temperature > FanOnTempThreshold && !_fan_enabled) {
      // Enable the fan relay
      _fan_relay_enable.setOutput(true);
      _fan_enabled = true;
    // Req 3. The controller shall disable the fan control relay when the temperature measurement is below 50C.
    } else if (temperature < FanOffTempThreshold && _fan_enabled) {
      // Disable the fan relay
      _fan_relay_enable.setOutput(false);
      _fan_enabled = false;
    }
  }
};