#pragma once

#include "card.hpp"
#include "common.hpp"
#include "robot.hpp"
class Level {
public:
  static constexpr u8 NUM_LEVELS = 2;

  static constexpr u8 MAX_ROBOTS = 8;
  static constexpr u8 MAX_BUTTONS = 8;
  static constexpr u8 MAX_PATHS = 8;
  static constexpr u8 MAX_BATTERIES = 8;
  static constexpr u8 MAX_CARDS = 7;

  static constexpr u8 ROWS = 10;
  static constexpr u8 COLUMNS = 16;

  static u8 map[ROWS * COLUMNS];
  static u8 energy[ROWS * COLUMNS];
  static Card script[MAX_CARDS];

  Coord entrance, exit;

  Robot robots[MAX_ROBOTS];
  u8 num_robots;

  char *metatiles;

  Coord *paths[MAX_PATHS];
  u8 num_paths;

  Level(const void *level_data);

  Robot &add_robot(Coord &coord);

  // computes the effective metatile for a given coordinate,
  // combining map array and metatiles pointer
  u8 effective_metatile(u8 index);
};

extern "C" const void *levels[];
