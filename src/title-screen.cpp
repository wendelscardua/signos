#include "title-screen.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "soundtrack.hpp"
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline)) TitleScreen::TitleScreen() {
  pal_bright(0);

  ppu_off();

  load_title_assets();

  oam_clear();

  scroll(0, 0);

  ppu_on_all();

  GGSound::play_song(Song::Lalala);

  pal_fade_to(0, 4);
}

__attribute__((noinline)) TitleScreen::~TitleScreen() {
  pal_fade_to(4, 0);
  ppu_off();
}

__attribute__((noinline)) void TitleScreen::loop() {
  while (current_game_state == GameState::TitleScreen) {
    ppu_wait_nmi();

    pad_poll(0);

    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_A | PAD_START)) {
      {
        GGSound::stop();
        current_game_state = GameState::LevelSelectScreen;
      }
    }
  }
}
