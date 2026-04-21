#include "signal.hpp"

Signal::Signal()
    : x(0.0_u8_8), y(0.0_u8_8), target_x(0.0_u8_8), target_y(0.0_u8_8),
      direction(Direction::East), source_robot_index(0), active(false) {}

Signal::~Signal() {}

void Signal::update() {
  if (!active) {
    return;
  }

  if (x < target_x) {
    x += SPEED;
    if (x >= target_x) {
      x = target_x;
      active = false;
    }
  } else if (x > target_x) {
    if (x <= target_x + SPEED) {
      x = target_x;
      active = false;
    } else {
      x -= SPEED;
    }
  }

  if (y < target_y) {
    y += SPEED;
    if (y >= target_y) {
      y = target_y;
      active = false;
    }
  } else if (y > target_y) {
    if (y <= target_y + SPEED) {
      y = target_y;
      active = false;
    } else {
      y -= SPEED;
    }
  }
}