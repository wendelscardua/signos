    .section .prg_rom_0,"aR",@progbits

    .global main_bg_palette, main_spr_palette, level_select_bg_palette, level_select_spr_palette, boot_bg_palette
    main_bg_palette: .incbin "main-bg.pal"
    main_spr_palette: .incbin "main-spr.pal"
    level_select_bg_palette: .incbin "level-select-bg.pal"
    level_select_spr_palette: .incbin "level-select-spr.pal"
    boot_bg_palette: .incbin "boot-bg.pal"

    .global title_nametable, level_select_nametable, boot_nametable
    title_nametable: .incbin "title.nam"
    level_select_nametable: .incbin "level-select.nam"
    boot_nametable: .incbin "boot-screen.nam"
    
    .section .chr_rom,"aR",@progbits
    .incbin "main-bg.chr"
    .incbin "main-spr.chr"
    .incbin "level-select-bg.chr"
    .incbin "level-select-spr.chr"