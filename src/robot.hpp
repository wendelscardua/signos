#pragma once

#include "common.hpp"
#include <fixed_point.h>

using namespace fixedpoint_literals;

class Robot {
public:
  enum State {
    Idle,
    Moving,
    Preparing,
    Executing,
  };

  static constexpr fu8_8 SPEED = 1.25_u8_8;

  State state;
  Coord coord;
  Direction direction;
  fu8_8 x, y;
  fu8_8 target_x, target_y;

  bool scripted;
  u8 script_index;

  Robot();
  void update();
};