#pragma once

#include "Drivers.h"

class TemperatureSensor {
public:
  TemperatureSensor(VoltageSensorInterface& temp_sensor_raw)
    : _voltageSensor(temp_sensor_raw) {}

  float getTemperature() {
    // Read voltage from the sensor
    float voltage = _voltageSensor.getVoltage();

    // Clamp voltage to the valid range
    voltage = clampVoltage(voltage);

    // Calculate temperature using the proportional ratio function
    float temperature = calculateTemperature(voltage);

    return temperature;
  }

private:
  VoltageSensorInterface& _voltageSensor;

  static constexpr float SensorMinVolt{0.25f};
  static constexpr float SensorSplyVolt{5.0f - SensorMinVolt};
  static constexpr float SensorCoeffA{1.375f}; // <-- Calib param Should not be constant
  static constexpr float SensorCoeffB{22.5f / 1000.0f};

  // if out of range values is persistant then diagnostics should take an action
  float clampVoltage(float voltage) {
    if (voltage < SensorMinVolt) {
      // TODO Set some  diagnostics
      return SensorMinVolt;
    } else if (voltage > SensorSplyVolt) {
      // TODO Set some  diagnostics
      return SensorSplyVolt;
    }
    return voltage;
  }

  // Calculate temperature using the proportional ratio function
  float calculateTemperature(float voltage) {
    return ((voltage - SensorCoeffA) / SensorCoeffB);
  }
};