#include "level-screen.hpp"
#include "attributes.hpp"
#include "banked-asset-helpers.hpp"
#include "ggsound.hpp"
#ifndef NDEBUG
#include "mesen-integration.hpp"
#endif
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
  for (u8 index = CARD_START_INDEX;
       index < CARD_START_INDEX + 2 * Level::MAX_CARDS; index++) {
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
  for (u8 index = CARD_START_INDEX + 0x10;
       index < CARD_START_INDEX + 0x10 + 2 * Level::MAX_CARDS; index++) {
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

void LevelScreen::handle_input(Robot &player, u8 &pressed, u8 &held) {
  switch (player.state) {
  case Robot::State::Idle:
    if (pressed & PAD_B) {
      player.state = Robot::State::Preparing;
      player.script_index = 0;
    }
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
        level.map[player.coord.index] &=
            ~(u8)(MapContent::SolidBit | MapContent::RobotBit);
        player.coord.index += (s8)player.direction;
        level.map[player.coord.index] |=
            (u8)(MapContent::SolidBit | MapContent::RobotBit);
      }
    }

    break;
  case Robot::State::Moving:
    break;
  case Robot::State::Preparing:
    if (pressed & PAD_UP) {
      if (player.script_index + level.script_nesting < Level::MAX_CARDS) {
        level.script[player.script_index] = Card::UpCard;
        draw_card(player.script_index);
        player.script_index++;
      }
    } else if (pressed & PAD_DOWN) {
      if (player.script_index + level.script_nesting < Level::MAX_CARDS) {
        level.script[player.script_index] = Card::DownCard;
        draw_card(player.script_index);
        player.script_index++;
      }
    } else if (pressed & PAD_LEFT) {
      if (player.script_index + level.script_nesting < Level::MAX_CARDS) {
        level.script[player.script_index] = Card::LeftCard;
        draw_card(player.script_index);
        player.script_index++;
      }
    } else if (pressed & PAD_RIGHT) {
      if (player.script_index + level.script_nesting < Level::MAX_CARDS) {
        level.script[player.script_index] = Card::RightCard;
        draw_card(player.script_index);
        player.script_index++;
      }
    } else if (pressed & PAD_B) {
      if (player.script_index + 2 * level.script_nesting < Level::MAX_CARDS) {
        level.script[player.script_index] = Card::PrepareCard;
        draw_card(player.script_index);
        player.script_index++;
        level.script_nesting++;
      }
    } else if (pressed & PAD_A) {
      if (level.script_nesting > 0) {
        level.script[player.script_index] = Card::SignalCard;
        draw_card(player.script_index);
        player.script_index++;
        level.script_nesting--;
      } else {
        player.state = Robot::State::Executing;
      }
    }
    break;
  case Robot::State::Executing:
    break;
  }
}
void LevelScreen::update_robots() {
  START_MESEN_WATCH("robots");
  for (u8 i = 0; i < level.num_robots; ++i) {
    level.robots[i].update();
  }
  STOP_MESEN_WATCH("robots");
}
void LevelScreen::update_paths() {
  START_MESEN_WATCH("paths");
  for (u8 i = 0; i < level.num_paths; ++i) {
    START_MESEN_WATCH("path");
    auto &path = level.paths[i];
    // first coord on the path is the button - batteries are along the way
    auto button_coord = path[0];
    if (level.energy[button_coord.index] == 0) {
      // if no energy, but a bot is on it, we should energize the path
      if (level.map[button_coord.index] & MapContent::RobotBit) {
        // energize the path
        for (u8 j = 0; path[j].index != 0xff; ++j) {
          auto coord = path[j];
          level.energy[coord.index]++;
          metatile_updates.enqueue(coord.index);
          if ((level.map[coord.index] & MapContent::BatteryBit) &&
              level.energy[coord.index] >= 3) {
            open_doors(coord.index);
          }
        }
      }
    } else {
      // if energy, but no bot is on it, we should de-energize the path
      if (!(level.map[button_coord.index] & MapContent::RobotBit)) {
        // de-energize the path
        for (u8 j = 0; path[j].index != 0xff; ++j) {
          auto coord = path[j];
          level.energy[coord.index]--;
          metatile_updates.enqueue(coord.index);
          if ((level.map[coord.index] & MapContent::BatteryBit) &&
              level.energy[coord.index] < 3) {
            close_doors(coord.index);
          }
        }
      }
    }
    STOP_MESEN_WATCH("path");
  }
  STOP_MESEN_WATCH("paths");
}
__attribute__((noinline)) void LevelScreen::loop() {
  Robot &player = level.robots[0];
  while (current_game_state == GameState::LevelScreen) {
    ppu_wait_nmi();

    START_MESEN_WATCH("level");
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
    if (metatile_updates.empty()) {
      handle_input(player, pressed, held);
      update_robots();
      update_paths();
    }
    update_metatiles();
    render_sprites();
    STOP_MESEN_WATCH("level");
  }
}

__attribute__((noinline)) void LevelScreen::render_sprites() {
  Robot &player = level.robots[0];
  {
    u8 *metasprite;
    switch (player.direction) {
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
    banked_oam_meta_spr(player.x.as_i(), player.y.as_i(), metasprite);
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
  if (player.state == Robot::State::Preparing) {
    if (player.script_index == Level::MAX_CARDS) {
      Coord card_position =
          (Coord)(CARD_START_INDEX + (Level::MAX_CARDS - 1) * 2);
      u8 x = card_position.column * 16;
      u8 y = card_position.row * 16;
      banked_oam_meta_spr(x, y, (u8 *)metasprite_WarningCursor);
    } else {
      Coord card_position = (Coord)(CARD_START_INDEX + player.script_index * 2);
      u8 x = card_position.column * 16;
      u8 y = card_position.row * 16;
      banked_oam_meta_spr(x, y, (u8 *)metasprite_WritingCursor);
    }
  }
  oam_hide_rest();
}

void LevelScreen::update_metatiles() {
  Attributes::enable_vram_buffer();
  for (u8 updates = 0; updates < 4 && !metatile_updates.empty(); ++updates) {
    u8 index = metatile_updates.dequeue();
    Coord coord = (Coord)index;
    auto metatile = level.effective_metatile(index);
    multi_vram_buffer_horz(
        (char[2]){metatiles_ul[metatile], metatiles_ur[metatile]}, 2,
        NTADR_A(coord.column * 2, coord.row * 2));
    multi_vram_buffer_horz(
        (char[2]){metatiles_dl[metatile], metatiles_dr[metatile]}, 2,
        NTADR_A(coord.column * 2, coord.row * 2 + 1));
    Attributes::set(coord.column, coord.row, metatiles_attr[metatile]);
  }
  Attributes::flush_vram_update();
}

void LevelScreen::open_doors(u8 index) {
  const s8 deltas[] = {-16, 16, -1, 1};
  for (auto delta : deltas) {
    u8 neighbor_index = (u8)(index + delta);
    if (level.map[neighbor_index] & MapContent::DoorBit) {
      level.energy[neighbor_index] = 3;
      metatile_updates.enqueue(neighbor_index);
    }
  }
}

void LevelScreen::close_doors(u8 index) {
  const s8 deltas[] = {-16, 16, -1, 1};
  for (auto delta : deltas) {
    u8 neighbor_index = (u8)(index + delta);
    if (level.map[neighbor_index] & MapContent::DoorBit) {
      level.energy[neighbor_index] = 0;
      metatile_updates.enqueue(neighbor_index);
    }
  }
}

void LevelScreen::draw_card(u8 card_index) {
  const u8 card_start = 0xc1;
  u8 index = card_start + card_index * 2;
  metatile_updates.enqueue(index);
  metatile_updates.enqueue(index + 1);
  metatile_updates.enqueue(index + 16);
  metatile_updates.enqueue(index + 17);
}