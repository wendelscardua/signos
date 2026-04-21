#pragma once

#include "common.hpp"
#include "levels.hpp"
#include "queue.hpp"

class LevelScreen {
public:
  Level level;
  Queue<u8, 32> metatile_updates;

  LevelScreen(u8 level);
  ~LevelScreen();

  void loop();

  void handle_input(Robot &player, u8 &held);
  void update_robots();
  void update_paths();

  void render_sprites();
  void update_metatiles();
};