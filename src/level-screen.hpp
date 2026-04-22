#pragma once

#include "common.hpp"
#include "levels.hpp"
#include "queue.hpp"

class LevelScreen {
public:
  static constexpr u8 CARD_START_INDEX = 0xc1;
  static constexpr u8 MAX_EXECUTION_STACK_SIZE = 4;
  u8 execution_stack[MAX_EXECUTION_STACK_SIZE];
  u8 execution_stack_index;
  u8 execution_index;

  Level level;
  u8 level_number;
  Queue<u8, 32> metatile_updates;

  LevelScreen(u8 level_number);
  ~LevelScreen();

  void loop();

  void handle_input(Robot &player, u8 &pressed, u8 &held);
  void draw_card(u8 index);
  void update_robots();
  void update_robot(Robot &robot);
  void update_paths();
  void update_signal();
  void open_doors(u8 index);
  void close_doors(u8 index);
  void render_sprites();
  void update_metatiles();

private:
  void clamp_robot_movement(Robot &robot);
  void send_signal(Robot &robot);
  void finish_robot_movement(Robot &robot);
  void finish_robot_program(Robot &robot);
};