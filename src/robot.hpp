#pragma once

#include "animation.hpp"
#include "card.hpp"
#include "common.hpp"
#include <fixed_point.h>

using namespace fixedpoint_literals;

class Robot {
public:
  enum State {
    Idle,
    Moving,
    Preparing,
    Signaling,
    Executing,
  };

  static constexpr fu8_8 SPEED = 1.25_u8_8;
  static constexpr u8 EXECUTION_FRAMES = 32;

  bool player;
  State state;
  Coord coord;
  Direction direction;
  fu8_8 x, y;
  fu8_8 target_x, target_y;

  Animation idle_up_animation{nullptr};
  Animation idle_down_animation{nullptr};
  Animation idle_left_animation{nullptr};
  Animation idle_right_animation{nullptr};
  Animation move_up_animation{nullptr};
  Animation move_down_animation{nullptr};
  Animation move_left_animation{nullptr};
  Animation move_right_animation{nullptr};

  Card *script_pointer;
  u8 script_index;
  u8 execution_frame_counter;

  Robot();
  void update();
  void move_up();
  void move_down();
  void move_left();
  void move_right();
  bool scripted();
  void render_sprite();
};