
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>

#include "TemperatureSensor.h"
#include "Drivers.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

class MockVoltageSensorIf : public VoltageSensorInterface {
public:
  float override{0.0f};
  float getVoltage() override {
    return override;
  }
};

TEST_CASE( "Testing Temperature Sensor" ) {

  MockVoltageSensorIf mockSensor;

  TemperatureSensor temperatureSensor(mockSensor);



  SECTION( "Check for Out of range min" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 0.001f;
    REQUIRE(-50.0f == Approx(temperatureSensor.getTemperature()));

  }

  SECTION( "Check for Minimum range" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 0.250f;
    REQUIRE(-50.0f == Approx(temperatureSensor.getTemperature()));

  }

  SECTION( "Check for 0°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 1.375f;
    REQUIRE(0.0f == Approx(temperatureSensor.getTemperature()));
  }

  SECTION( "Check for 50°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 2.5f;
    REQUIRE(50.0f == Approx(temperatureSensor.getTemperature()));
  }

  SECTION( "Check for 100°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 3.625f;
    REQUIRE(100.0f == Approx(temperatureSensor.getTemperature()));
  }

  SECTION( "Check for Max Temp 150°C" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 4.75f;
    REQUIRE(150.0f == Approx(temperatureSensor.getTemperature()));
  }

  SECTION( "Check for Out of Range Voltage (5v)" ) {
    // Set expectations for mock sensor readings
    mockSensor.override = 5.1f;
    REQUIRE(150.0f == Approx(temperatureSensor.getTemperature()));
  }

}