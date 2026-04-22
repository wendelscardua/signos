#include "banked-asset-helpers.hpp"
#include "assets.hpp"
#include "bank-helper.hpp"
#include <mapper.h>
#include <neslib.h>

__attribute__((noinline, section(".prg_rom_fixed"))) void load_title_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(main_bg_palette);
  pal_spr(main_spr_palette);
}

__attribute__((noinline, section(".prg_rom_fixed"))) void
load_level_select_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(level_select_bg_palette);
  pal_spr(level_select_spr_palette);
}

__attribute__((noinline, section(".prg_rom_fixed"))) void load_level_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(main_bg_palette);
  pal_spr(main_spr_palette);
}

__attribute__((noinline, section(".prg_rom_0"))) void load_title_assets() {
  set_chr_mode_0(0);
  set_chr_mode_1(2);
  set_chr_mode_2(4);
  set_chr_mode_3(5);
  set_chr_mode_4(6);
  set_chr_mode_5(7);
  vram_adr(NAMETABLE_A);
  vram_write((void *)title_nametable, 1024);
  load_title_palette();
}

__attribute__((noinline, section(".prg_rom_0"))) void
load_level_select_assets() {
  set_chr_mode_0(8);
  set_chr_mode_1(10);
  set_chr_mode_2(12);
  set_chr_mode_3(13);
  set_chr_mode_4(14);
  set_chr_mode_5(15);

  vram_adr(NAMETABLE_A);
  vram_write((void *)level_select_nametable, 1024);
  load_level_select_palette();
}

__attribute__((noinline, section(".prg_rom_0"))) void load_level_assets() {
  set_chr_mode_0(0);
  set_chr_mode_1(2);
  set_chr_mode_2(4);
  set_chr_mode_3(5);
  set_chr_mode_4(6);
  set_chr_mode_5(7);
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, 1024);
  load_level_palette();
}