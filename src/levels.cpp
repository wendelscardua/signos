#include "levels.hpp"
#include "common.hpp"

__attribute__((
    section(".prg_ram.noinit"))) u8 Level::map[Level::ROWS * Level::COLUMNS];
__attribute__((
    section(".prg_ram.noinit"))) u8 Level::energy[Level::ROWS * Level::COLUMNS];

Level::Level(const void *level_data) : num_robots(0), num_paths(0) {
  const void *cursor = level_data;

  // first "robot" is the player (has special treatment, like input, entrance
  // cutscene, etc.)
  Coord &robotCoords = *(Coord *)cursor;
  auto &player = add_robot(robotCoords);

  entrance = player.coord;

  cursor = (Coord *)(cursor) + 1;

  u8 read_robots = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;
  for (u8 i = 0; i < read_robots; i++) {
    Coord &robotCoords = *(Coord *)cursor;
    add_robot(robotCoords);
    cursor = (Coord *)(cursor) + 1;
  }

  Coord &exitCoords = *(Coord *)cursor;
  exit = exitCoords;
  cursor = (Coord *)(cursor) + 1;

  metatiles = (char *)cursor;

  cursor = (char *)(cursor) + ROWS * COLUMNS;

  u8 read_paths = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;
  for (u8 i = 0; i < read_paths; i++) {
    paths[i] = (Coord *)cursor;
    cursor = (Coord *)(cursor) + 1;
  }

  for (u8 index = 0; index < ROWS * COLUMNS; index++) {
    u8 metatile = *(char *)(metatiles + index);
    map[index] = EmptyBit;
    energy[index] = 0;

    if (metatile == 0x19 || metatile == 0x1a) { // closed door
      map[index] |= MapContent::SolidBit | MapContent::DoorBit;
    }

    if (metatile == 0x03) { // glass pane
      map[index] |= MapContent::SolidBit;
    }

    if (metatile == 0x14 || metatile == 0x15 || metatile == 0x16 ||
        metatile == 0x17) { // batteries
      map[index] |= MapContent::SolidBit;
      energy[index] = metatile - 0x14;
    }

    if (metatile == 0x02) { // wall
      map[index] |= MapContent::SolidBit;
    }
  }

  for (u8 i = 0; i < num_robots; i++) {
    map[robots[i].coord.index] |= MapContent::SolidBit;
  }
}

Robot &Level::add_robot(Coord &coord) {
  robots[num_robots].coord = coord;
  robots[num_robots].direction = Direction::East;
  robots[num_robots].x = 16.0_u8_8 * coord.column;
  robots[num_robots].y = 16.0_u8_8 * coord.row;
  robots[num_robots].target_x = robots[num_robots].x;
  robots[num_robots].target_y = robots[num_robots].y;
  map[coord.index] |= MapContent::SolidBit;
  num_robots++;
  return robots[num_robots - 1];
}

u8 Level::effective_metatile(u8 index) {
  auto metatile = metatiles[index];
  auto map_content = map[index];
  auto energy_content = energy[index];

  if (energy_content > 0) {
    if (map_content >= 0x05 && map_content <= 0x13) { // cables and buttons
      metatile += 0x20; // lit up metatiles are 0x20 below the unlit ones
    } else if (map_content >= 0x14 && map_content <= 0x17) { // batteries
      metatile =
          0x14 + energy_content; // battery metatiles are sorted by energy value
      if (metatile > 0x17) {
        metatile = 0x17;
      }
    }
  } else if (index == entrance.index) {
    metatile = 0x1c;
  } else if (index == exit.index) {
    metatile = 0x1d;
  }

  return metatile;
}
