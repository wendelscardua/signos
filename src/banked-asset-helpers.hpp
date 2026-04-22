
#pragma once

// draws metasprite from the metasprite bank w/ vertical scroll culling
extern "C" void banked_oam_meta_spr(char x, int y, const void *data);

// draws metasprite from the metasprite bank w/ horizontal scroll culling
extern "C" void banked_oam_meta_spr_horizontal(int x, char y, const void *data);

// [assets bank] loads title assets
void load_title_assets();

// [assets bank] loads level select assets
void load_level_select_assets();

// [assets bank] loads level assets
void load_level_assets();

// [assets bank] loads boot assets
void load_boot_assets();