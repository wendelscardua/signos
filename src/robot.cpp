#include "robot.hpp"
#include "animation-defs.hpp"
#include "bank-helper.hpp"

Robot::Robot()
    : player(false), coord(0), direction(Direction::East), x(0.0_u8_8),
      y(0.0_u8_8), target_x(0.0_u8_8), target_y(0.0_u8_8),
      script_pointer(nullptr), execution_frame_counter(0) {
  idle_up_animation = Animation{&idle_up_cells};
  idle_down_animation = Animation{&idle_down_cells};
  idle_left_animation = Animation{&idle_left_cells};
  idle_right_animation = Animation{&idle_right_cells};
  move_up_animation = Animation{&move_up_cells};
  move_down_animation = Animation{&move_down_cells};
  move_left_animation = Animation{&move_left_cells};
  move_right_animation = Animation{&move_right_cells};
  exit_level_animation = Animation{&exit_level_cells};
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
  bool reskin = true;
  switch (state) {
  case State::Idle:
  case State::Preparing:
  case State::Signaling:
  case State::Executing:
    switch (direction) {
    case Direction::East:
      animation = &idle_right_animation;
      break;
    case Direction::West:
      animation = &idle_left_animation;
      break;
    case Direction::North:
      animation = &idle_up_animation;
      break;
    case Direction::South:
      animation = &idle_down_animation;
      break;
    }
    break;
  case State::Moving:
    switch (direction) {
    case Direction::East:
      animation = &move_right_animation;
      break;
    case Direction::West:
      animation = &move_left_animation;
      break;
    case Direction::North:
      animation = &move_up_animation;
      break;
    case Direction::South:
      animation = &move_down_animation;
      break;
    }
    break;
  case State::ExitingLevel:
    animation = &exit_level_animation;
    reskin = false;
    break;
  }
  if (animation != nullptr) {
    banked_lambda(1, [&]() {
      if (player && reskin) {
        animation->reskin_update(x.as_i(), y.as_i() + GAMEPLAY_SCROLL_Y);
      } else {
        animation->update(x.as_i(), y.as_i() + GAMEPLAY_SCROLL_Y);
      }
    });
  }
}