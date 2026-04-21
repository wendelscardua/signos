#include "robot.hpp"

Robot::Robot()
    : coord(0), direction(Direction::East), x(0.0_u8_8), y(0.0_u8_8),
      target_x(0.0_u8_8), target_y(0.0_u8_8), script_pointer(nullptr),
      execution_frame_counter(0) {}

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