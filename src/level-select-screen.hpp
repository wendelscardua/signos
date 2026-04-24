#pragma once

#include "common.hpp"

class LevelSelectScreen {
public:
  u8 &selected_level;
  LevelSelectScreen(u8 &selected_level);
  ~LevelSelectScreen();
  void loop();

private:
  void refresh_high_scores();
};