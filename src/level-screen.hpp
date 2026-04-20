#pragma once

#include "common.hpp"
#include "levels.hpp"

class LevelScreen {
public:
  Level level;
  LevelScreen(u8 level);
  ~LevelScreen();
  void loop();

  void render_sprites();
};