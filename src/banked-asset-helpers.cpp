#include "banked-asset-helpers.hpp"
#include "assets.hpp"
#include "bank-helper.hpp"
#include <mapper.h>
#include <neslib.h>

__attribute__((noinline, section(".prg_rom_fixed"))) void
banked_play_song(Song song) {
  ScopedBank scopedBank(GET_BANK(song_list));
  GGSound::play_song(song);
}

__attribute__((noinline, section(".prg_rom_fixed"))) void
banked_play_sfx(SFX sfx, GGSound::SFXPriority priority) {
  ScopedBank scopedBank(GET_BANK(instrument_list));
  GGSound::play_sfx(sfx, priority);
}

__attribute__((noinline, section(".prg_rom_fixed"))) void banked_stop_song() {
  ScopedBank scopedBank(GET_BANK(song_list));
  GGSound::stop();
}

__attribute__((noinline, section(".prg_rom_fixed"))) void load_title_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(bg_palette);
  pal_spr(spr_palette);
}

__attribute__((noinline, section(".prg_rom_0"))) void load_title_assets() {
  vram_adr(NAMETABLE_A);
  vram_write((void *)title_nametable, 1024);
  load_title_palette();
}
