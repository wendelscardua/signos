#pragma once

#include "common.hpp"

class Player {
public:
  Coord coord;
  u8 keys;
  SpawnIndex generation;
  Facing facing;

  Player();
};

class Pushable {
public:
  Coord coord;
  PushableType type;

  // for altars
  SpawnIndex last_generation;

  Pushable();
};

class Door {
public:
  Coord coord;
};

class Key {
public:
  Coord coord;
};

class Button {
public:
  Coord coord;
};

class Spawner {
public:
  Coord coord;
  SpawnIndex index;
  bool used;
};

struct SpawnerData {
  Coord coord;
  SpawnIndex index;
};

struct PushableData {
  Coord coord;
  PushableType type;
};

class Level {
public:
  static constexpr u8 NUM_LEVELS = 8;

  static constexpr u8 MAX_PLAYERS = 8;
  static constexpr u8 MAX_PUSHABLES = 12;
  static constexpr u8 MAX_DOORS = 4;
  static constexpr u8 MAX_SPAWNERS = 8;
  static constexpr u8 MAX_BUTTONS = 8;

  static constexpr u8 ROWS = 15;
  static constexpr u8 COLUMNS = 16;

  static constexpr u16 SPAWNER_BIT_PER_GENERATION[] = {
      0, Spawner1Bit, Spawner2Bit, Spawner3Bit};

  static constexpr u16 SPAWNER_BIT_FLIP_PER_GENERATION[] = {
      0, Spawner1Bit | UsedSpawner1Bit, Spawner2Bit | UsedSpawner2Bit,
      Spawner3Bit | UsedSpawner3Bit};

  static u16 map[ROWS * COLUMNS];

  Coord goal;

  Player players[MAX_PLAYERS];
  u8 num_players;

  Pushable pushables[MAX_PUSHABLES];
  u8 num_pushables;

  Door doors[MAX_DOORS];
  u8 num_doors;

  Key keys[MAX_DOORS];
  u8 num_keys;

  Spawner spawners[MAX_SPAWNERS];
  u8 num_spawners;

  Button buttons[MAX_BUTTONS];
  u8 num_buttons;

  char *metatiles;

  SpawnIndex current_generation;

  Level(const void *level_data);

  Player &add_player(Coord &coord, SpawnIndex generation);
  void delete_player(Coord &coord);

  void add_pushable(PushableData &data);
  void add_door(Coord &coord);
  void add_key(Coord &key);
  void add_spawner(SpawnerData &data);
  void add_button(Coord &coord);

  // computes the effective metatile for a given coordinate,
  // combining map array and metatiles pointer
  u8 effective_metatile(u8 index);

  // removes key at coord
  void get_key(Coord &coord);
};

extern "C" const void *levels[];
extern u8 best_moves[Level::NUM_LEVELS];
extern u8 stars[Level::NUM_LEVELS];
