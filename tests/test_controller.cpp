
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>

#include "FanController.h"
#include "Drivers.h"

#include <cstdlib>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

class MockPwmIf : public PwmOutputInterface {
public:
  float output_duty;
  void setOutputDuty(float duty) override {
    output_duty = duty;
  }
};

class MockGpioIf : public GpioOutputInterface {
public:
  bool gpio_state{false};
  void setOutput(bool state) override {
    gpio_state = state;
  }
};

class MockVoltageSensorIf : public VoltageSensorInterface {
public:
  float override{0.0f};
  float getVoltage() override {
    return override;
  }
};

TEST_CASE( "Testing" ) {

  MockVoltageSensorIf mockSensor;
  MockGpioIf mockSensorPower;
  MockGpioIf mockFanRelay;
  MockPwmIf  mockFanSpeed;

  TemperatureSensor temperatureSensor(mockSensor);
  // Create the FanController
  FanController controller(temperatureSensor, mockSensorPower, mockFanRelay, mockFanSpeed);

  // Initialize the controller if needed
  controller.init();

  SECTION("FanController Sensor initialization") {
    // You can add initialization tests here if needed
    REQUIRE(true == mockSensorPower.gpio_state);
  }
 
  SECTION( "Check for Out of range min" ) {
    // Set expectations for mock sensor readings
    //REQUIRE_CALL(mockSensor, getVoltage()).RETURN(1.0f); // Adjust voltage as needed

    mockSensor.override = 0.001f;
    // Call the controller's loop method
    controller.loop();
    REQUIRE(false == mockFanRelay.gpio_state);
    REQUIRE(0.0f == Approx(mockFanSpeed.output_duty));

    // assertions based on the expected behavior of the controller
    //REQUIRE_CALL(mockFanRelay, setOutput(true)); // Expect fan relay to be turned on
    
    // assertions based on the expected behavior of the controller
    //REQUIRE_CALL(mockFanSpeed, setOutputDuty(0.0f)); // Expect fan relay to be turned on

  }

  SECTION( "Check for Minimum range" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 0.250f;

    controller.loop();
    REQUIRE(false == mockFanRelay.gpio_state);
    REQUIRE(0.0f == Approx(mockFanSpeed.output_duty));

  }

  SECTION( "Check for 50°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 2.50f;

    controller.loop();
    REQUIRE(false == mockFanRelay.gpio_state);
    REQUIRE(0.0f == Approx(mockFanSpeed.output_duty));

  }

  SECTION( "Check for 60°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 2.73f;

    controller.loop();
    REQUIRE(true == mockFanRelay.gpio_state);
    REQUIRE(5.1111f == Approx(mockFanSpeed.output_duty));

  }

  SECTION( "Check for 99°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 3.61f;

    controller.loop();
    REQUIRE(true == mockFanRelay.gpio_state);
    REQUIRE(24.6667f == Approx(mockFanSpeed.output_duty));

  }

  SECTION( "Check for 100°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 3.625f;

    controller.loop();
    REQUIRE(true == mockFanRelay.gpio_state);
    REQUIRE(25.0f == Approx(mockFanSpeed.output_duty));

  }

  SECTION( "Check for 130°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 4.3f;

    controller.loop();
    REQUIRE(true == mockFanRelay.gpio_state);
    REQUIRE(90.0f == Approx(mockFanSpeed.output_duty));

  }
  SECTION( "Check for Max Temp 140°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 4.6f;

    controller.loop();
    REQUIRE(true == mockFanRelay.gpio_state);
    REQUIRE(100.0f == Approx(mockFanSpeed.output_duty));
  }

  SECTION( "Check for Out of Range Voltage (5v)" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 5.1f;

    controller.loop();
    REQUIRE(true == mockFanRelay.gpio_state);
    REQUIRE(100.0f == Approx(mockFanSpeed.output_duty));
  }

}