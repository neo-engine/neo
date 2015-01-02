/*
Pokémon Emerald 2 Version
------------------------------

file        : fs.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>

#include <nds.h>

#include "sprite.h"
#include "saveGame.h"
#include "print.h"

extern unsigned int NAV_DATA[ 12288 ];
extern unsigned short NAV_DATA_PAL[ 256 ];
extern SavMod savMod;

namespace FS {

#define MAXITEMSPERPAGE 12

    std::string readString( FILE*, bool p_new = false );
    std::wstring readWString( FILE*, bool p_new = false );

    std::string breakString( const std::string& p_string, u8 p_lineLength ); 
    std::string breakString( const std::string& p_string, FONT::font p_font, u8 p_lineLength );

    bool loadNavScreen( u16* p_layer, const char* p_name, u8 p_no );
    bool loadPicture( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256 );
    bool loadPictureSub( u16* p_layer, const char* p_path, const char* p_name, u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256 );
    bool loadSprite( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt );
    bool loadSpriteSub( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt );
    bool loadPKMNSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const u16& p_pkmnNo, const s16 p_posX,
                         const s16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_shiny = false, bool p_female = false, bool p_flipX = false );
    bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const u16& p_pkmnNo, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_shiny = false, bool p_female = false, bool p_flipX = false );

    bool loadTrainerSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_flipX = false );
    bool loadTrainerSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u16 p_posX,
                               const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_flipX = false );

    void drawItem( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const u16 p_posX, const u16 p_posY, const u16 p_cnt,
                   u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen, bool p_showcnt );
    void drawItemIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const u16 p_posX, const u16 p_posY,
                       u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen = true );
    void drawPKMNIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const u16& p_pkmnNo, const u16 p_posX, const u16 p_posY,
                       u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen = true );
    void drawEggIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const u16 p_posX, const u16 p_posY,
                      u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen = true );
    void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16 & p_nextAvailableTileIdx, Type p_type, u16 p_posX, u16 p_posY, bool p_bottom );

    void drawItem( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const u16 p_posX, const u16 p_posY, const u16 p_cnt,
                   u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen = true, bool p_showcnt = false );

    const char* getLoc( u16 p_ind );
}