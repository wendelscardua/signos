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
  Robot &player = level.robots[0];
  while (current_game_state == GameState::LevelScreen) {
    ppu_wait_nmi();
    pad_poll(0);
    u8 held = pad_state(0);
    u8 pressed = get_pad_new(0);
    if (pressed & PAD_SELECT) {
      {
        GGSound::stop();
        current_game_state = GameState::TitleScreen;
        // TODO: just reset level instead?
      }
    }
    switch (player.state) {
    case Robot::State::Idle:
      if (held & PAD_UP) {
        player.state = Robot::State::Moving;
        player.direction = Direction::North;
        player.target_x = player.x;
        player.target_y = player.y - 16;
      }
      if (held & PAD_DOWN) {
        player.state = Robot::State::Moving;
        player.direction = Direction::South;
        player.target_x = player.x;
        player.target_y = player.y + 16;
      }
      if (held & PAD_LEFT) {
        player.state = Robot::State::Moving;
        player.direction = Direction::West;
        player.target_x = player.x - 16;
        player.target_y = player.y;
      }
      if (held & PAD_RIGHT) {
        player.state = Robot::State::Moving;
        player.direction = Direction::East;
        player.target_x = player.x + 16;
        player.target_y = player.y;
      }
      if (player.state == Robot::State::Moving) {
        if ((player.coord.index + (s8)player.direction < 0 ||
             player.coord.index + (s8)player.direction >=
                 Level::ROWS * Level::COLUMNS ||
             (level.map[player.coord.index + (s8)player.direction] &
              MapContent::SolidBit) != 0)) {
          player.state = Robot::State::Idle;
          player.target_x = player.x;
          player.target_y = player.y;
        } else {
          level.map[player.coord.index] &= ~MapContent::SolidBit;
          player.coord.index += (s8)player.direction;
          level.map[player.coord.index] |= MapContent::SolidBit;
        }
      }
      break;
    case Robot::State::Moving:
      break;
    case Robot::State::Preparing:
      break;
    case Robot::State::Executing:
      break;
    }
    for (u8 i = 0; i < level.num_robots; ++i) {
      level.robots[i].update();
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
    banked_oam_meta_spr(robot.x.as_i(), robot.y.as_i(), metasprite);
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
    banked_oam_meta_spr(robot.x.as_i(), robot.y.as_i(), metasprite);
  }
  oam_hide_rest();
}