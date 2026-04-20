#include "level-screen.hpp"
#include "attributes.hpp"
#include "banked-asset-helpers.hpp"
#include "ggsound.hpp"
#include "metatiles.hpp"
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline)) LevelScreen::LevelScreen(u8 level_number)
    : level(levels[level_number]) {

  pal_bright(0);

  ppu_off();

  load_level_assets();

  oam_clear();

  scroll(0, 0);

  for (u8 index = 0; index < Level::ROWS * Level::COLUMNS; ++index) {
    Coord coord = (Coord)index;
    u8 metatile = level.effective_metatile(index);
    vram_adr((unsigned int)NTADR_A(coord.column * 2, coord.row * 2));
    vram_put(metatiles_ul[metatile]);
    vram_put(metatiles_ur[metatile]);
    vram_adr((unsigned int)NTADR_A(coord.column * 2, coord.row * 2 + 1));
    vram_put(metatiles_dl[metatile]);
    vram_put(metatiles_dr[metatile]);
    Attributes::set(coord.column, coord.row, metatiles_attr[metatile]);
  }

  Attributes::update_vram();

  ppu_on_all();

  pal_fade_to(0, 4);
}

__attribute__((noinline)) LevelScreen::~LevelScreen() {
  pal_fade_to(4, 0);
  ppu_off();
}

__attribute__((noinline)) void LevelScreen::loop() {
  while (current_game_state == GameState::LevelScreen) {
    ppu_wait_nmi();
    pad_poll(0);
    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_A | PAD_START)) {
      {
        GGSound::stop();
        current_game_state = GameState::TitleScreen;
      }
    }
  }
}