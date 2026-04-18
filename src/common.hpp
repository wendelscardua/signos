#pragma once

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

union Coord {
  u8 index;
  struct {
    u8 column : 4;
    u8 row : 4;
  };
};

enum class Direction : s8 {
  North = -16,
  South = +16,
  East = -1,
  West = +1,
};

enum class SpawnIndex : u8 {
  Zero,
  One,
  Two,
  Three,
};

enum class PushableType : u8 {
  Altar,
  Crystal,
};

enum class Facing : u8 {
  Left,
  Right,
};

enum MapContent : u16 {
  EmptyBit = 0,
  SolidBit = 1,
  KeyBit = 2,
  DoorBit = 4,
  AltarBit = 8,
  CrystalBit = 16,
  PlayerBit = 32,
  Spawner1Bit = 64,
  Spawner2Bit = 128,
  Spawner3Bit = 256,
  UsedSpawner1Bit = 512,
  UsedSpawner2Bit = 1024,
  UsedSpawner3Bit = 2048,
  ButtonBit = 4096,
  SolidSet = SolidBit | DoorBit | PlayerBit,
  PushableSet = AltarBit | CrystalBit,
};

enum class GameState : u8 {
  TitleScreen,
};

extern GameState current_game_state;

// neslib/nesdoug internal stuff

extern "C" char OAM_BUF[256];
extern "C" char SPRID;

extern u8 VRAM_INDEX;
extern char VRAM_BUF[256];
