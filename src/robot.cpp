#include "robot.hpp"
#include "animation-defs.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "mesen-integration.hpp"
#include "metasprites.hpp"
#include <string.h>

Robot::Robot()
    : player(false), coord(0), direction(Direction::East), x(0.0_u8_8),
      y(0.0_u8_8), target_x(0.0_u8_8), target_y(0.0_u8_8),
      script_pointer(nullptr), execution_frame_counter(0) {
  idle_up = Animation{&idle_up_cells};
  idle_down = Animation{&idle_down_cells};
  idle_left = Animation{&idle_left_cells};
  idle_right = Animation{&idle_right_cells};
}

bool Robot::scripted() { return script_pointer != nullptr; }

void Robot::move_up() {
  state = State::Moving;
  direction = Direction::North;
  target_x = x;
  target_y = y - 16;
}

void Robot::move_down() {
  state = State::Moving;
  direction = Direction::South;
  target_x = x;
  target_y = y + 16;
}

void Robot::move_left() {
  state = State::Moving;
  direction = Direction::West;
  target_x = x - 16;
  target_y = y;
}

void Robot::move_right() {
  state = State::Moving;
  direction = Direction::East;
  target_x = x + 16;
  target_y = y;
}

void Robot::render_sprite() {
  Animation *animation = nullptr;
  switch (state) {
  case State::Idle:
    switch (direction) {
    case Direction::East:
      animation = &idle_right;
      break;
    case Direction::West:
      animation = &idle_left;
      break;
    case Direction::North:
      animation = &idle_up;
      break;
    case Direction::South:
      animation = &idle_down;
      break;
    }
    break;
  }
  if (animation != nullptr) {
    banked_lambda(1, [&]() {
      if (player) {
        animation->reskin_update(x.as_i(), y.as_i());
      } else {
        animation->update(x.as_i(), y.as_i());
      }
    });
  }
}