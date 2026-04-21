#include "robot.hpp"

Robot::Robot()
    : coord(0), direction(Direction::East), x(0.0_u8_8), y(0.0_u8_8),
      target_x(0.0_u8_8), target_y(0.0_u8_8), scripted(false), script_index(0) {
}

void Robot::update() {
  switch (state) {
  case State::Idle:
    break;
  case State::Moving:
    if (x < target_x) {
      x += SPEED;
      if (x >= target_x) {
        x = target_x;
        state = State::Idle;
      }
    } else if (x > target_x) {
      x -= SPEED;
      if (x <= target_x) {
        x = target_x;
        state = State::Idle;
      }
    } else if (y < target_y) {
      y += SPEED;
      if (y >= target_y) {
        y = target_y;
        state = State::Idle;
      }
    } else if (y > target_y) {
      y -= SPEED;
      if (y <= target_y) {
        y = target_y;
        state = State::Idle;
      }
    }
    break;
  case State::Preparing:
    break;
  case State::Executing:
    break;
  }
}