#pragma once

#include "common.hpp"
#include <soa.h>

const int PPU_PATTERN_TABLE_0 = 0x0000;
const int PPU_PATTERN_TABLE_1 = 0x1000;

// prg bank index where most assets are stored
const u8 ASSETS_BANK = 0;
// prg bank for palettes
const u8 PALETTES_BANK = 0;

extern "C" const char main_bg_palette[];
extern "C" const char main_spr_palette[];
extern "C" const char level_select_bg_palette[];
extern "C" const char level_select_spr_palette[];
extern "C" const char boot_bg_palette[];
extern "C" const char title_nametable[];
extern "C" const char level_select_nametable[];
extern "C" const char boot_nametable[];