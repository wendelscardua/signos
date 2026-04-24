#include "bank-helper.hpp"
#include "boot-screen.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "level-screen.hpp"
#include "level-select-screen.hpp"
#include "soundtrack-ptr.hpp"
#include "title-screen.hpp"
#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>
#include <peekpoke.h>

GameState current_game_state;

__attribute__((section(".prg_ram.noinit"))) volatile u32 signature;

__attribute__((
    section(".prg_ram.noinit"))) volatile u8 level_completed[NUM_LEVELS];

__attribute__((
    section(".prg_ram.noinit"))) volatile u8 best_time_minutes[NUM_LEVELS];
__attribute__((
    section(".prg_ram.noinit"))) volatile u8 best_time_seconds[NUM_LEVELS];
__attribute__((section(".prg_ram.noinit"))) volatile u16 best_steps[NUM_LEVELS];

static void main_init() {
  set_prg_8000(0);
  set_wram_mode(WRAM_ON);

  disable_irq();

  ppu_off();

  set_mirroring(MIRROR_VERTICAL);

  // set 8x16 sprite mode
  oam_size(1);

  bank_bg(0);
  bank_spr(1);

  set_vram_buffer();

  current_game_state = GameState::BootScreen;

  {
    {
      ScopedBank bank(GGSound::BANK);
      GGSound::init(GGSound::Region::NTSC, song_list, sfx_list,
                    instrument_list);
    }
  }

  if (signature != SIGNATURE) {
    // reset save ram
    signature = SIGNATURE;
    for (u8 i = 0; i < NUM_LEVELS; i++) {
      level_completed[i] = false;
      best_time_minutes[i] = 99;
      best_time_seconds[i] = 99;
      best_steps[i] = 9999;
    }
  }

  asm volatile("cli");
}

int main() {
  main_init();

  u8 selected_level = 0;

  while (true) {
    switch (current_game_state) {
    case GameState::BootScreen: {
      BootScreen boot_screen;
      boot_screen.loop();
    } break;
    case GameState::TitleScreen: {
      TitleScreen title_screen;
      title_screen.loop();
    } break;
    case GameState::LevelSelectScreen: {
      LevelSelectScreen level_select_screen(selected_level);
      level_select_screen.loop();
    } break;
    case GameState::LevelScreen: {
      LevelScreen level_screen(selected_level);
      level_screen.loop();
    } break;
    }
  }
}
