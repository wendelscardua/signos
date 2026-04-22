#pragma once

#include "common.hpp"
class BootScreen {
public:
  BootScreen();
  ~BootScreen();
  void loop();

private:
  void tick(u8 *ram_counter);
};