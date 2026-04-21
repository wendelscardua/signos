#include "level-screen.hpp"
#include "attributes.hpp"
#include "banked-asset-helpers.hpp"
#include "ggsound.hpp"
#include "metasprites.hpp"
#include "metatiles.hpp"
#include "robot.hpp"
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline)) LevelScreen::LevelScreen(u8 level_number)
    : level(levels[level_number]) {

  pal_bright(0);

  ppu_off();

  load_level_assets();

  oam_clear();

  scroll(0, 0);

  execution_stack_index = 0;
  execution_index = 0xff;

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
      player.move_up();
    }
    if (held & PAD_DOWN) {
      player.move_down();
    }
    if (held & PAD_LEFT) {
      player.move_left();
    }
    if (held & PAD_RIGHT) {
      player.move_right();
    }
    clamp_robot_movement(player);

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
        level.signal.script_index = 0;
        send_signal(player);
      }
    }
    break;
  case Robot::State::Signaling:
    break;
  case Robot::State::Executing:
    break;
  }
}
void LevelScreen::update_robots() {
  for (u8 i = 0; i < level.num_robots; ++i) {
    update_robot(level.robots[i]);
  }
}

void LevelScreen::update_paths() {
  for (u8 i = 0; i < level.num_paths; ++i) {
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
  }
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
    if (metatile_updates.empty()) {
      handle_input(player, pressed, held);
      update_robots();
      update_paths();
      update_signal();
    }
    update_metatiles();
    render_sprites();
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
    if (player.state == Robot::State::Executing) {
      execution_index = player.script_index;
    }
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
    if (robot.state == Robot::State::Executing) {
      execution_index = robot.script_index;
    }
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
  } else if (execution_index != 0xff) {
    if (execution_index == Level::MAX_CARDS) {
      Coord card_position =
          (Coord)(CARD_START_INDEX + (Level::MAX_CARDS - 1) * 2);
      u8 x = card_position.column * 16;
      u8 y = card_position.row * 16;
      banked_oam_meta_spr(x, y, (u8 *)metasprite_ProcessingCursor);
    } else {
      Coord card_position = (Coord)(CARD_START_INDEX + execution_index * 2);
      u8 x = card_position.column * 16;
      u8 y = card_position.row * 16;
      banked_oam_meta_spr(x, y, (u8 *)metasprite_ProcessingCursor);
    }
  }
  if (level.signal.active) {
    u8 x = level.signal.x.as_i();
    u8 y = level.signal.y.as_i();
    banked_oam_meta_spr(x, y, (u8 *)metasprite_Signal);
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
      level.map[neighbor_index] &= ~MapContent::SolidBit;
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
      level.map[neighbor_index] |= MapContent::SolidBit;
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

void LevelScreen::update_signal() {
  if (level.signal.active) {
    level.signal.update();
    if (!level.signal.active) {
      Robot &source = level.robots[level.signal.source_robot_index];
      // signal has reached its target
      // if the target is a robot, it loads the script and executes it
      if (level.signal.target_robot_index != 0xff) {
        Robot &target = level.robots[level.signal.target_robot_index];
        target.script_pointer = level.script + level.signal.script_index;
        target.script_index = level.signal.script_index;
        target.state = Robot::State::Executing;
        target.execution_frame_counter = 0;
        execution_stack[execution_stack_index++] =
            level.signal.source_robot_index;
      } else {
        // otherwise, it fizzles and the source robot returns to idle
        source.state = Robot::State::Idle;
        for (u8 i = level.signal.script_index; i < source.script_index; ++i) {
          level.script[i] = Card::EmptyCard;
          draw_card(i);
        }
      }
      finish_robot_movement(source); // fizzling a running program resumes it
    }
  }
}

void LevelScreen::update_robot(Robot &robot) {
  switch (robot.state) {
  case Robot::State::Idle:
    break;
  case Robot::State::Moving:
    if (robot.x < robot.target_x) {
      robot.x += Robot::SPEED;
      if (robot.x >= robot.target_x) {
        robot.x = robot.target_x;
        robot.state = Robot::State::Idle;
      }
    } else if (robot.x > robot.target_x) {
      if (robot.x <= robot.target_x + Robot::SPEED) {
        robot.x = robot.target_x;
        robot.state = Robot::State::Idle;
      } else {
        robot.x -= Robot::SPEED;
      }
    } else if (robot.y < robot.target_y) {
      robot.y += Robot::SPEED;
      if (robot.y >= robot.target_y) {
        robot.y = robot.target_y;
        robot.state = Robot::State::Idle;
      }
    } else if (robot.y > robot.target_y) {
      if (robot.y <= robot.target_y + Robot::SPEED) {
        robot.y = robot.target_y;
        robot.state = Robot::State::Idle;
      } else {
        robot.y -= Robot::SPEED;
      }
    }
    finish_robot_movement(robot);
    break;
  case Robot::State::Preparing:
    break;
  case Robot::State::Signaling:
    break;
  case Robot::State::Executing:
    robot.execution_frame_counter++;
    if (robot.execution_frame_counter >= Robot::EXECUTION_FRAMES) {
      robot.execution_frame_counter = 0;
      if (robot.scripted()) {
        Card card = *robot.script_pointer;
        switch (card) {
        case Card::UpCard:
          if (robot.direction != Direction::North) {
            robot.direction = Direction::North;
            finish_robot_movement(robot);
          } else {
            robot.move_up();
            clamp_robot_movement(robot);
          }
          break;
        case Card::DownCard:
          if (robot.direction != Direction::South) {
            robot.direction = Direction::South;
            finish_robot_movement(robot);
          } else {
            robot.move_down();
            clamp_robot_movement(robot);
          }
          break;
        case Card::LeftCard:
          if (robot.direction != Direction::West) {
            robot.direction = Direction::West;
            finish_robot_movement(robot);
          } else {
            robot.move_left();
            clamp_robot_movement(robot);
          }
          break;
        case Card::RightCard:
          if (robot.direction != Direction::East) {
            robot.direction = Direction::East;
            finish_robot_movement(robot);
          } else {
            robot.move_right();
            clamp_robot_movement(robot);
          }
          break;
        case Card::PrepareCard:
          level.script[robot.script_index] = Card::EmptyCard;
          draw_card(robot.script_index);
          level.signal.source_robot_index = (u8)(&robot - level.robots);
          level.signal.script_index = ++robot.script_index;
          robot.script_pointer++;
          {
            u8 recursion_stack_size = 0;
            while (robot.script_index < Level::MAX_CARDS &&
                   (level.script[robot.script_index] != Card::SignalCard ||
                    recursion_stack_size > 0)) {
              if (level.script[robot.script_index] == Card::PrepareCard) {
                recursion_stack_size++;
              } else if (recursion_stack_size > 0 &&
                         level.script[robot.script_index] == Card::SignalCard) {
                recursion_stack_size--;
              }
              robot.script_pointer++;
              robot.script_index++;
            }
          }
          break;
        case Card::SignalCard:
          send_signal(robot);
          break;
        case Card::EmptyCard:
          finish_robot_program(robot);
          break;
        }
      }
    }
    break;
  }
}

void LevelScreen::finish_robot_movement(Robot &robot) {
  if (!robot.scripted()) {
    return;
  }
  if (robot.state != Robot::Idle && robot.state != Robot::State::Executing &&
      robot.state != Robot::State::Signaling) {
    return;
  }
  if ((robot.state == Robot::State::Idle ||
       robot.state == Robot::State::Executing)) {
    robot.state = Robot::State::Executing;
  }
  robot.execution_frame_counter = 0;
  level.script[robot.script_index] = Card::EmptyCard;
  draw_card(robot.script_index);
  robot.script_pointer++;
  robot.script_index++;
}

void LevelScreen::clamp_robot_movement(Robot &robot) {
  if (robot.state == Robot::State::Moving) {
    if ((robot.coord.index + (s8)robot.direction < 0 ||
         robot.coord.index + (s8)robot.direction >=
             Level::ROWS * Level::COLUMNS ||
         (level.map[robot.coord.index + (s8)robot.direction] &
          MapContent::SolidBit) != 0)) {
      robot.state = Robot::State::Idle;
      robot.target_x = robot.x;
      robot.target_y = robot.y;
      finish_robot_movement(robot);
    } else {
      level.map[robot.coord.index] &=
          ~(u8)(MapContent::SolidBit | MapContent::RobotBit);
      robot.coord.index += (s8)robot.direction;
      level.map[robot.coord.index] |=
          (u8)(MapContent::SolidBit | MapContent::RobotBit);
    }
  }
}

void LevelScreen::send_signal(Robot &robot) {
  robot.state = Robot::State::Signaling;
  // initialize signal at robot's position
  level.signal.active = true;
  level.signal.source_robot_index = (u8)(&robot - level.robots);
  level.signal.target_robot_index = 0xff;
  level.signal.direction = robot.direction;
  level.signal.x = robot.x;
  level.signal.y = robot.y;

  // find first robot in the direction of the signal, or obstacle
  Coord target_coord = (Coord)((u8)(robot.coord.index + (s8)robot.direction));

  // keep walking while the next coord is inbounds and not a solid (or is
  // solid but it's a glass pane)
  while (target_coord.index >= 0 &&
         target_coord.index < Level::ROWS * Level::COLUMNS &&
         ((level.map[target_coord.index] & MapContent::SolidBit) == 0 ||
          level.effective_metatile((u8)(target_coord.index)) == 0x03)) {
    target_coord.index += (s8)robot.direction;
  }
  level.signal.target_x = 16.0_u8_8 * target_coord.column;
  level.signal.target_y = 16.0_u8_8 * target_coord.row;

  // if the target coord has a robot, set the target index
  if (level.map[target_coord.index] & MapContent::RobotBit) {
    for (u8 i = 0; i < level.num_robots; ++i) {
      if (level.robots[i].coord.index == target_coord.index) {
        level.signal.target_robot_index = i;
        break;
      }
    }
  }
}

void LevelScreen::finish_robot_program(Robot &robot) {
  robot.state = Robot::State::Idle;
  robot.script_pointer = nullptr;
  robot.script_index = 0;
  u8 other_index = execution_stack[--execution_stack_index];
  auto &other = level.robots[other_index];
  if (other.scripted()) {
    other.state = Robot::State::Executing;
  } else {
    other.state = Robot::State::Idle;
    execution_index = 0xff;
  }
}