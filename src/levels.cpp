#include "levels.hpp"
#include "common.hpp"

Player::Player() : keys(0), facing(Facing::Right) {}

Pushable::Pushable() : last_generation(SpawnIndex::Zero) {}

__attribute__((
    section(".prg_ram.noinit"))) u16 Level::map[Level::ROWS * Level::COLUMNS];

Level::Level(const void *level_data)
    : num_players(0), num_pushables(0), num_doors(0), num_keys(0),
      num_spawners(0), num_buttons(0), current_generation(SpawnIndex::Zero) {
  const void *cursor = level_data;
  Coord &playerCoords = *(Coord *)cursor;
  add_player(playerCoords, SpawnIndex::Zero);
  cursor = (Coord *)(cursor) + 1;

  goal = *(Coord *)cursor;
  cursor = (Coord *)(cursor) + 1;

  u8 read_pushables = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;

  for (u8 i = 0; i < read_pushables; i++) {
    PushableData &data = *(PushableData *)cursor;
    cursor = (PushableData *)(cursor) + 1;
    add_pushable(data);
  }

  u8 read_doors = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;

  for (u8 i = 0; i < read_doors; i++) {
    Coord &doorCoords = *(Coord *)cursor;
    cursor = (Coord *)(cursor) + 1;
    add_door(doorCoords);
  }

  u8 read_keys = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;

  for (u8 i = 0; i < read_keys; i++) {
    Coord &keyCoords = *(Coord *)cursor;
    cursor = (Coord *)(cursor) + 1;
    add_key(keyCoords);
  }

  u8 read_spawners = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;

  for (u8 i = 0; i < read_spawners; i++) {
    SpawnerData &data = *(SpawnerData *)cursor;
    cursor = (SpawnerData *)(cursor) + 1;
    add_spawner(data);
  }

  u8 read_buttons = *(u8 *)cursor;
  cursor = (u8 *)(cursor) + 1;

  for (u8 i = 0; i < read_buttons; i++) {
    Coord &button_coord = *(Coord *)cursor;
    cursor = (Coord *)(cursor) + 1;
    add_button(button_coord);
  }

  metatiles = (char *)cursor;

  for (u8 index = 0; index < ROWS * COLUMNS; index++) {
    u8 metatile = *(char *)(metatiles + index);
    // XXX: mts 1,2,4,5 are always solid, 3 doesn't appear on maps
    map[index] =
        (metatile > 0 && metatile < 6 ? MapContent::SolidBit : EmptyBit);
  }

  for (u8 i = 0; i < num_players; i++) {
    map[players[i].coord.index] |= MapContent::PlayerBit;
  }

  for (u8 i = 0; i < num_doors; i++) {
    map[doors[i].coord.index] |= MapContent::DoorBit;
  }

  for (u8 i = 0; i < num_keys; i++) {
    map[keys[i].coord.index] |= MapContent::KeyBit;
  }

  for (u8 i = 0; i < num_pushables; i++) {
    if (pushables[i].type == PushableType::Altar) {
      map[pushables[i].coord.index] |= MapContent::AltarBit;
    } else {
      map[pushables[i].coord.index] |= MapContent::CrystalBit;
    }
  }

  for (u8 i = 0; i < num_spawners; i++) {
    Spawner &spawner = spawners[i];
    map[spawner.coord.index] |=
        Level::SPAWNER_BIT_PER_GENERATION[(u8)spawner.index];
  }

  for (u8 i = 0; i < num_buttons; i++) {
    map[buttons[i].coord.index] |= MapContent::ButtonBit;
  }
}

Player &Level::add_player(Coord &coord, SpawnIndex generation) {
  players[num_players].coord = coord;
  players[num_players].generation = generation;
  players[num_players].keys = 0;
  map[coord.index] ^= PlayerBit;
  num_players++;
  return players[num_players - 1];
}

void Level::delete_player(Coord &coord) {
  for (u8 i = 0; i < num_players; i++) {
    if (players[i].coord.index == coord.index) {
      map[players[i].coord.index] ^= PlayerBit;
      num_players--;
      players[i] = players[num_players];
      i--;
    }
  }
}

void Level::add_pushable(PushableData &data) {
  pushables[num_pushables].coord = data.coord;
  pushables[num_pushables].type = data.type;
  num_pushables++;
}

void Level::add_spawner(SpawnerData &data) {
  spawners[num_spawners].coord = data.coord;
  spawners[num_spawners].index = data.index;
  spawners[num_spawners].used = false;
  num_spawners++;
}

void Level::add_door(Coord &coord) {
  doors[num_doors].coord = coord;
  num_doors++;
}

void Level::add_key(Coord &coord) {
  keys[num_keys].coord = coord;
  num_keys++;
}

void Level::add_button(Coord &coord) {
  buttons[num_buttons].coord = coord;
  num_buttons++;
}

u8 Level::effective_metatile(u8 index) {
  auto metatile = metatiles[index];
  auto map_content = map[index];
  if (map_content & DoorBit) {
    metatile = 3;
  } else if (map_content & AltarBit) {
    metatile = 11;
    if ((map[index - 1] | map[index - 16] | map[index + 1] | map[index + 16]) &
        CrystalBit) {
      metatile = 27;
    }
  } else if (map_content & CrystalBit) {
    metatile = 10;
    if ((map[index - 1] | map[index - 16] | map[index + 1] | map[index + 16]) &
        AltarBit) {
      metatile = 26;
    }
  } else if (map_content & Spawner1Bit) {
    metatile = 6;
  } else if (map_content & Spawner2Bit) {
    metatile = 7;
  } else if (map_content & Spawner3Bit) {
    metatile = 8;
  } else if (map_content & UsedSpawner1Bit) {
    metatile = 22;
  } else if (map_content & UsedSpawner2Bit) {
    metatile = 23;
  } else if (map_content & UsedSpawner3Bit) {
    metatile = 24;
  } else if (map_content & ButtonBit) {
    metatile = 13;
  } else if (index == goal.index) {
    metatile = 12;
  }

  return metatile;
}

void Level::get_key(Coord &coord) {
  for (u8 i = 0; i < num_keys; i++) {
    if (keys[i].coord.index == coord.index) {
      map[keys[i].coord.index] ^= KeyBit;
      num_keys--;
      keys[i] = keys[num_keys];
      break;
    }
  }
}
