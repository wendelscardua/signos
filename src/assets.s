    .section .prg_rom_0,"aR",@progbits

    .global main_bg_palette, main_spr_palette
    main_bg_palette: .incbin "main-bg.pal"
    main_spr_palette: .incbin "main-spr.pal"

    .global title_nametable
    title_nametable: .incbin "title.nam"
    
    .section .chr_rom,"aR",@progbits
    .incbin "main-bg.chr"
    .incbin "main-spr.chr"