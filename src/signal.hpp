#pragma once

#include "common.hpp"
#include <fixed_point.h>

using namespace fixedpoint_literals;

class Signal {
public:
  static constexpr fu8_8 SPEED = 1.5_u8_8;
  fu8_8 x, y;
  fu8_8 target_x, target_y;
  Direction direction;
  u8 source_robot_index;
  u8 target_robot_index;
  u8 script_index;

  bool active;

  Signal();
  ~Signal();

  void update();
};