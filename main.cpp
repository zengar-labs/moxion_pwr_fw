
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>

#include "FanController.h"
#include "Drivers.h"

/**
 * @brief Example of how parameters will be passed to your Controller and how it will be executed
 */
void run_test_demo_code(VoltageSensorInterface& temp_sensor_raw,
                        GpioOutputInterface& sensor_power_enable,
                        GpioOutputInterface& fan_relay_enable,
                        PwmOutputInterface& fan_output_raw) 
{
    static float constexpr v_MAX{5.0f}; // step voltage in volts
    static float constexpr v_step{0.025f}; // step voltage in volts
    float voltage_injection{0.0f}; // voltage to be injected in volts
    // Create an instance of TemperatureSensor, passing the voltage sensor as a parameter
    TemperatureSensor temperatureSensor(temp_sensor_raw);
    /** TODO: instantiate and initialize FanController **/
    FanController controller(temperatureSensor, sensor_power_enable, fan_relay_enable, fan_output_raw);

    std::chrono::minutes loopDuration(1);
    auto startTime = std::chrono::steady_clock::now();

    // Initialize the controller if needed
    controller.init();
    std::cout << std::fixed << std::setprecision(1);

    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::minutes>(currentTime - startTime);
        if (elapsedTime >= loopDuration) {
            break;
        }

    
        /** TODO: call FanController.loop(); **/
        controller.loop();
        // A goo way to test is creating some MOCKS for the get Voltage in order to inject values  less than 0v, or more than supply voltage (error values). Then supply in range values and check if fan is enable and duty cycle.
        // As I dont have gmock on my system, I thought on two options, catch2 (needs some modifications on the interface) / do a swap of values (SIL) (Not for production but fun to see the output on console, easy to validate visually)
    #ifdef UT_TESTING 
        std::cout << "Temperature: " << std::setw(3) << temperatureSensor.getTemperature() << "  Relay: " << std::setw(3) << \
          (fan_relay_enable.gpio_state ? "ON" : "OFF") << " Duty Cycle: " << std::setw(3) << fan_output_raw.duty_cycle_state << " \r" << std::flush;
    #endif
        if (voltage_injection >= v_MAX)
          break;
        else
          voltage_injection += v_step;
        temp_sensor_raw.override = voltage_injection;
     
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  
    }

}

int main() {
    VoltageSensorInterface temp_sensor_raw;
    GpioOutputInterface sensor_power_enable;
    GpioOutputInterface fan_relay_enable;
    PwmOutputInterface fan_output_raw;

    run_test_demo_code(temp_sensor_raw, sensor_power_enable, fan_relay_enable, fan_output_raw);
    return 0;
}