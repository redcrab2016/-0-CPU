    include "bs3core.inc"
    include "bs3gfx.inc"

    ; ROM resources
    embed   "sk_scr_death.bs3",     0,  0
    embed   "sk_scr_end.bs3",       1,  0
    embed   "sk_scr_logo.bs3",      2,  0
    embed   "sk_scr_hv.bs3",        3,  0
    embed   "sk_scr_intro1.bs3",    4,  0
    embed   "sk_scr_intro2.bs3",    5,  0
    embed   "sk_scr_menu.bs3",      6,  0
    embed   "sk_map0.bs3",          7,  0
    embed   "sk_map1.bs3",          8,  0
    embed   "sk_map2.bs3",          9,  0   
    embed   "sk_map3.bs3",          10, 0
    embed   "sk_map4.bs3",          11, 0
    embed   "sk_map5.bs3",          12, 0
    embed   "sk_map6.bs3",          13, 0
    embed   "sk_map7.bs3",          14, 0
    embed   "sk_map8.bs3",          15, 0
    embed   "sk_map9.bs3",          16, 0
    embed   "sk_mapa.bs3",          17, 0
    embed   "sk_mapb.bs3",          18, 0
    embed   "sk_mapc.bs3",          19, 0
    embed   "sk_mapd.bs3",          20, 0
    embed   "sk_mape.bs3",          21, 0
    embed   "sk_spr_data.bs3",      22, 0
    embed   "sk_spr_mask.bs3",      23, 0
    embed   "sk_tile_data.bs3",     24, 0
    embed   "sk_tile_mask.bs3",     25, 0
    embed   "sk_fonts.bs3",         26, 0

    mbs3_bootat start

start:
    mbs3_gfx_reset
    

    mbs3_gfx_end
    hlt
