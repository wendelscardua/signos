#include "level-screen.hpp"
#include "attributes.hpp"
#include "banked-asset-helpers.hpp"
#include "ggsound.hpp"
#include "metasprites.hpp"
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
    render_sprites();
  }
}

__attribute__((noinline)) void LevelScreen::render_sprites() {
  {
    Robot &robot = level.robots[0];
    u8 *metasprite;
    switch (robot.direction) {
    case Direction::East:
      metasprite = (u8 *)metasprite_RobotRight;
      break;
    case Direction::West:
      metasprite = (u8 *)metasprite_RobotLeft;
      break;
    case Direction::North:
      metasprite = (u8 *)metasprite_RobotUp;
      break;
    case Direction::South:
      metasprite = (u8 *)metasprite_RobotDown;
      break;
    default:
      metasprite = (u8 *)metasprite_RobotRight;
      break;
    }
    banked_oam_meta_spr(robot.coord.column * 16, robot.coord.row * 16,
                        metasprite);
  }
  for (u8 index = 1; index < level.num_robots; ++index) {
    Robot &robot = level.robots[index];
    u8 *metasprite;
    switch (robot.direction) {
    case Direction::East:
      metasprite = (u8 *)metasprite_OtherRobotRight;
      break;
    case Direction::West:
      metasprite = (u8 *)metasprite_OtherRobotLeft;
      break;
    case Direction::North:
      metasprite = (u8 *)metasprite_OtherRobotUp;
      break;
    case Direction::South:
      metasprite = (u8 *)metasprite_OtherRobotDown;
      break;
    default:
      metasprite = (u8 *)metasprite_OtherRobotRight;
      break;
    }
    banked_oam_meta_spr(robot.coord.column * 16, robot.coord.row * 16,
                        metasprite);
  }
  oam_hide_rest();
}