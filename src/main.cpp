#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "levels.hpp"
#include "soundtrack-ptr.hpp"
#include "title-screen.hpp"
#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>
#include <peekpoke.h>

GameState current_game_state;
u8 best_moves[Level::NUM_LEVELS];
u8 stars[Level::NUM_LEVELS];

static void main_init() {
  set_prg_8000(0);
  set_wram_mode(WRAM_ON);

  disable_irq();

  ppu_off();

  // set 8x16 sprite mode
  oam_size(1);

  bank_bg(0);
  bank_spr(1);
  set_chr_mode_0(0);
  set_chr_mode_1(2);
  set_chr_mode_2(4);
  set_chr_mode_3(5);
  set_chr_mode_4(6);
  set_chr_mode_5(7);

  set_vram_buffer();

  load_title_assets();

  current_game_state = GameState::TitleScreen;

  {
    {
      ScopedBank bank(GGSound::BANK);
      GGSound::init(GGSound::Region::NTSC, song_list, sfx_list,
                    instrument_list);
    }
  }

  for (u8 i = 0; i < Level::NUM_LEVELS; i++) {
    stars[i] = 0;
    best_moves[i] = 0x99;
  }

  asm volatile("cli");
}

int main() {
  main_init();

  while (true) {
    switch (current_game_state) {
    case GameState::TitleScreen: {
      TitleScreen title_screen;
      title_screen.loop();
    } break;
    }
  }
}
