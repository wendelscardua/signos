#include "animation.hpp"
#include "banked-asset-helpers.hpp"
#include <string.h>

// same as metasprites
#pragma clang section text = ".prg_rom_1.text.animation"
#pragma clang section rodata = ".prg_rom_1.rodata.animation"

bool Animation::paused = false;

Animation::Animation(const AnimCell (*cells)[])
    : finished(false), current_cell(&(*cells)[0]), cells(cells) {}

void Animation::reset() {
  current_cell = &(*cells)[0];
  finished = false;
}

void Animation::update(char x, int y) {
  banked_oam_meta_spr((char)(x + current_cell->delta_x),
                      (char)(y + current_cell->delta_y),
                      current_cell->metasprite);
  if (paused) {
    return;
  }
  current_cell++;
  if (current_cell->flags == 0) {
    current_cell = &(*cells)[0];
    finished = true;
  }
}

void Animation::reskin_update(char x, int y) {
  // size of robot metasprites (25 using 8x8, 17 using 8x16)
  static constexpr u8 ROBOT_METASPRITE_SIZE = 17;

  u8 reskin_metasprite[ROBOT_METASPRITE_SIZE];
  memcpy(reskin_metasprite, current_cell->metasprite, ROBOT_METASPRITE_SIZE);
  for (u8 index = 3; index < ROBOT_METASPRITE_SIZE; index += 4) {
    // change palette to 01
    reskin_metasprite[index] = (reskin_metasprite[index] & ~0b11) | 0b01;
  }
  banked_oam_meta_spr((char)(x + current_cell->delta_x),
                      (char)(y + current_cell->delta_y), reskin_metasprite);
  if (paused) {
    return;
  }
  current_cell++;
  if (current_cell->flags == 0) {
    current_cell = &(*cells)[0];
    finished = true;
  }
}

u8 Animation::current_cell_flags() const { return current_cell->flags; }
bool Animation::current_cell_flags(u8 flags) const {
  return flags & current_cell->flags;
}