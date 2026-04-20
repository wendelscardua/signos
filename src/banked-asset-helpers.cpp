#include "banked-asset-helpers.hpp"
#include "assets.hpp"
#include "bank-helper.hpp"
#include <mapper.h>
#include <neslib.h>

__attribute__((noinline, section(".prg_rom_fixed"))) void load_title_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(bg_palette);
  pal_spr(spr_palette);
}

__attribute__((noinline, section(".prg_rom_fixed"))) void load_level_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(bg_palette);
  pal_spr(spr_palette);
}

__attribute__((noinline, section(".prg_rom_0"))) void load_title_assets() {
  vram_adr(NAMETABLE_A);
  vram_write((void *)title_nametable, 1024);
  load_title_palette();
}

__attribute__((noinline, section(".prg_rom_0"))) void load_level_assets() {
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);
  load_level_palette();
}