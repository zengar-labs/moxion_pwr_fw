#pragma once

/**
 * @brief Interface for getting a raw voltage sensor measurement
 */
class VoltageSensorInterface {

public:
  /**
   * @brief Get the most recent voltage measurement
   * @return Latest voltage measurement [0 12]
   */
  virtual float getVoltage() { 
    if (override == 0.0f)
      return 0.0f; //return hal_adc_get_voltage();
    
    return override;
   };
   float override{0.0f};
};

/**
 * @brief Interface for commanding a PWM output
 */
class PwmOutputInterface {

public:
  /**
   * @brief Set the Output Duty object
   * @param duty Duty cycle in percent [0 100] 
   */
  virtual void setOutputDuty(float duty) {
    duty_cycle_state = duty;
  };
  float duty_cycle_state{0.0f};
};

/**
 * @brief Interface for commanding a GPIO output
 */
class GpioOutputInterface {

public:
  /**
   * @brief Set the Output object
   * @param output True to enable output, False to disable output
   */
  virtual void setOutput(bool output) { gpio_state = output; };
  bool gpio_state{false};
};
