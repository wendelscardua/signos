#include "level-select-screen.hpp"
#include "attributes.hpp"
#include "banked-asset-helpers.hpp"
#include "metasprites.hpp"
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline))
LevelSelectScreen::LevelSelectScreen(u8 &selected_level)
    : selected_level(selected_level) {

  pal_bright(0);

  ppu_off();

  load_level_select_assets();

  Attributes::init();

  oam_clear();

  for (u8 i = 0; i < NUM_LEVELS; i++) {
    u8 folder_column = i % 4;
    u8 folder_row = i / 4;

    u8 meta_x = folder_column * 3 + 2;
    u8 meta_y = folder_row * 3 + 3;

    if (level_completed[i]) {
      vram_adr((unsigned int)NTADR_A(meta_x * 2, meta_y * 2));
      vram_write((char[]){0x86, 0x87}, 2);
      vram_adr((unsigned int)NTADR_A(meta_x * 2, meta_y * 2 + 1));
      vram_write((char[]){0x96, 0x97}, 2);
      Attributes::set(meta_x, meta_y, 0b01010101); // green check color
    } else if (!level_completed[0] && i > 0) {
      vram_adr((unsigned int)NTADR_A(meta_x * 2, meta_y * 2));
      vram_write((char[]){0x88, 0x89}, 2);
      vram_adr((unsigned int)NTADR_A(meta_x * 2, meta_y * 2 + 1));
      vram_write((char[]){0x98, 0x99}, 2);
      Attributes::set(meta_x, meta_y, 0b11111111); // red crossed out color
    }
  }

  Attributes::update_vram();

  scroll(0, 0);

  ppu_on_all();

  pal_fade_to(0, 4);
}

__attribute__((noinline)) LevelSelectScreen::~LevelSelectScreen() {
  pal_fade_to(4, 0);
  ppu_off();
}

__attribute__((noinline)) void LevelSelectScreen::loop() {
  while (current_game_state == GameState::LevelSelectScreen) {
    ppu_wait_nmi();

    pad_poll(0);

    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_A | PAD_START)) {
      {
        if (selected_level == 0 || level_completed[0]) {
          current_game_state = GameState::LevelScreen;
        } else {
          // TODO: deny with sfx?
        }
      }
    }
    if (pressed & PAD_UP) {
      if (selected_level < 4) {
        selected_level += 8;
      } else {
        selected_level -= 4;
      }
    }
    if (pressed & PAD_DOWN) {
      if (selected_level >= NUM_LEVELS - 4) {
        selected_level -= 8;
      } else {
        selected_level += 4;
      }
    }
    if (pressed & PAD_LEFT) {
      if (selected_level > 0) {
        selected_level--;
      } else {
        selected_level = NUM_LEVELS - 1;
      }
    }
    if (pressed & PAD_RIGHT) {
      if (selected_level < NUM_LEVELS - 1) {
        selected_level++;
      } else {
        selected_level = 0;
      }
    }

    u8 cursor_x = (selected_level % 4 * 3 + 2) * 16 + 8;
    u8 cursor_y = (selected_level / 4 * 3 + 3) * 16 + 4;

    banked_oam_meta_spr(cursor_x, cursor_y, (u8 *)Metasprites::MouseCursor);
    oam_hide_rest();
  }
}
