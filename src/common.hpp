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
  East = +1,
  West = -1,
};

enum MapContent : u8 {
  EmptyBit = 0,
  SolidBit = 1,
  DoorBit = 2,
  ButtonBit = 4,
  RobotBit = 8,
  BatteryBit = 16,
};

enum class GameState : u8 {
  BootScreen,
  TitleScreen,
  LevelSelectScreen,
  LevelScreen,
};

const u32 SIGNATURE = 0x31415926;
const u8 NUM_LEVELS = 12;

extern GameState current_game_state;
extern volatile u8 level_completed[NUM_LEVELS];

// neslib/nesdoug internal stuff

extern "C" char OAM_BUF[256];
extern "C" char SPRID;

extern u8 VRAM_INDEX;
extern char VRAM_BUF[256];
