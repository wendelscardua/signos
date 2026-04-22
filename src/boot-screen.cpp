#include "boot-screen.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "utils.hpp"
#include <nesdoug.h>
#include <neslib.h>
#include <string.h>

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

  while (current_game_state == GameState::BootScreen) {
    ppu_wait_nmi();
    ram_counter += 9;
    if (ram_counter > 2048) {
      ram_counter = 2048;
    }
    u8 ram_counter_text[4];
    int_to_text(ram_counter_text, ram_counter);
    multi_vram_buffer_horz(ram_counter_text, 4, NTADR_A(16, 7));

    pad_poll(0);
    u8 pressed = get_pad_new(0);
    if (pressed || ram_counter == 2048) {
      {
        current_game_state = GameState::TitleScreen;
      }
    }
  }
}