#pragma once

#include "common.hpp"
#include "levels.hpp"
#include "queue.hpp"

class LevelScreen {
public:
  static constexpr u8 CARD_START_INDEX = 0xc1;
  Level level;
  Queue<u8, 32> metatile_updates;

  LevelScreen(u8 level);
  ~LevelScreen();

  void loop();

  void handle_input(Robot &player, u8 &pressed, u8 &held);
  void draw_card(u8 index);
  void update_robots();
  void update_paths();
  void open_doors(u8 index);
  void close_doors(u8 index);
  void render_sprites();
  void update_metatiles();
};