#pragma once

#include "common.hpp"

struct AnimCell {
  const unsigned char *metasprite;
  const u8 flags;
};

class Animation {
public:
  static bool paused;
  bool finished;

  Animation(const AnimCell (*cells)[]);

  void reset();

  void update(char x, int y);
  void reskin_update(char x, int y); // XXX: for player sprites

  u8 current_cell_flags() const;
  bool current_cell_flags(u8 flags) const;

private:
  const AnimCell *current_cell;
  const AnimCell (*cells)[];
};