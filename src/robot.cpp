#include "robot.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "mesen-integration.hpp"
#include "metasprites.hpp"
#include <string.h>

Robot::Robot()
    : player(false), coord(0), direction(Direction::East), x(0.0_u8_8),
      y(0.0_u8_8), target_x(0.0_u8_8), target_y(0.0_u8_8),
      script_pointer(nullptr), execution_frame_counter(0) {}

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
  u8 *metasprite;
  u8 reskin_metasprite[25];

  switch (direction) {
  case Direction::East:
    metasprite = (u8 *)metasprite_RobotRightIdle1;
    break;
  case Direction::West:
    metasprite = (u8 *)metasprite_RobotLeftIdle1;
    break;
  case Direction::North:
    metasprite = (u8 *)metasprite_RobotUpIdle1;
    break;
  case Direction::South:
    metasprite = (u8 *)metasprite_RobotDownIdle1;
    break;
  default:
    metasprite = (u8 *)metasprite_RobotRightIdle1;
    break;
  }
  if (player) {
    banked_lambda(1, [&]() { memcpy(reskin_metasprite, metasprite, 25); });
    for (u8 index = 3; index < 25; index += 4) {
      // change palette
      reskin_metasprite[index] = (reskin_metasprite[index] & ~0b11) | 0b01;
    }
    metasprite = reskin_metasprite;
  }
  banked_oam_meta_spr(x.as_i(), y.as_i(), metasprite);
}