#pragma once

#include "Drivers.h"
/**
 * @brief Example FanController base
 */
class FanController {
public:

  /*
   * @brief One time executed initialization method
   */
  void init() {};
  /**
   * @brief Periodic loop method that will be called every 500ms
   */
  void loop() {};
};