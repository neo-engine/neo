/*
Pokémon Emerald 2 Version
------------------------------

file        : ability.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2014
Philip Wellnitz (RedArceus)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:


1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
is required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/

#pragma once

#include <string>

#include <nds.h>

#include "sprite.h"
#include "saveGame.h"

extern unsigned int NAV_DATA[ 12288 ];
extern unsigned short NAV_DATA_PAL[ 256 ];
extern u8 BG_ind;
extern SavMod savMod;

namespace FS {

    std::string readString( FILE*, bool p_new = false );
    std::wstring readWString( FILE*, bool p_new = false );

    std::string breakString( const std::string& p_string, int p_lineLength );

    bool loadNavScreen( u16* p_layer, const char* p_name, u8 p_no );
    bool loadPicture( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256 );
    bool loadPictureSub( u16* p_layer, const char* p_path, const char* p_name, u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256 );
    bool loadSprite( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt );
    bool loadSpriteSub( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt );
    bool loadPKMNSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const u16& p_pkmnNo, const u16 p_posX,
                         const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_shiny = false, bool p_female = false, bool p_flipX = false );
    bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const u16& p_pkmnNo, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_shiny = false, bool p_female = false, bool p_flipX = false );

    bool loadTrainerSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_flipX = false );
    bool loadTrainerSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u16 p_posX,
                               const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_nextAvailableTileIdx, bool p_bottom, bool p_flipX = false );

    const char* getLoc( u16 p_ind );
}