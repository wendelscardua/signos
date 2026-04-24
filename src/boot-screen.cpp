#include "boot-screen.hpp"
#include "banked-asset-helpers.hpp"
#include "charset.hpp"
#include "common.hpp"
#include "utils.hpp"
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline)) BootScreen::BootScreen() {
  pal_bright(0);
  ppu_off();
  load_boot_assets();
  oam_clear();
  scroll(0, 0);
  ppu_on_all();
  pal_fade_to(0, 4);
}

__attribute__((noinline)) BootScreen::~BootScreen() {
  pal_fade_to(4, 0);
  ppu_off();
}

__attribute__((noinline)) void BootScreen::loop() {
  u16 ram_counter = 0;
  u8 wait = 0;

  while (current_game_state == GameState::BootScreen) {
    ppu_wait_nmi();
    ram_counter += 13;
    if (ram_counter > 2048) {
      ram_counter = 2048;
      wait++;
      if (wait >= 60) {
        wait = 60;
      }
    }

    if (wait < 2) {
      u8 ram_counter_text[4];
      int_to_text(ram_counter_text, ram_counter);
      multi_vram_buffer_horz(ram_counter_text, 4, NTADR_A(16, 7));
    }
    if (wait == 1) {
      multi_vram_buffer_horz("Starting signOS"_ts, 15, NTADR_A(2, 11));
    }

    pad_poll(0);
    u8 pressed = get_pad_new(0);
    if (pressed || wait >= 60) {
      {
        current_game_state = GameState::TitleScreen;
      }
    }
  }
}