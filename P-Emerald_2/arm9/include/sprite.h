/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : sprite.h
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

#include <nds.h>

#include "Normal.h"
#include "Kampf.h"
#include "Flug.h"
#include "Gift.h"
#include "Boden.h"
#include "Gest.h"
#include "Kaefer.h"
#include "Geist.h"
#include "Stahl.h"
#include "Unbek.h"
#include "Wasser.h"
#include "Feuer.h"
#include "Pfl.h"
#include "Elek.h"
#include "Psycho.h"
#include "Eis.h"
#include "Dra.h"
#include "Unl.h"
#include "Fee.h"

#include "type.h"

extern const unsigned int* TypeTiles[ 19 ];
extern const unsigned short* TypePals[ 19 ];
extern const unsigned int* HitTypeTiles[ 3 ];
extern const unsigned short* HitTypePals[ 3 ];

static const u8 SPRITE_DMA_CHANNEL = 2;

#define BYTES_PER_16_COLOR_TILE 32
#define COLORS_PER_PALETTE      16
#define BOUNDARY_VALUE          32
static const u16 OFFSET_MULTIPLIER = ( BOUNDARY_VALUE / sizeof( SPRITE_GFX[ 0 ] ) );
static const u16 OFFSET_MULTIPLIER_SUB = ( BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] ) );

struct SpriteInfo {
    u8 m_oamId;
    u8 m_width;
    u8 m_height;
    u16 m_angle;
    SpriteEntry * m_entry;
};

/*
*  updateOAM
*
*  Update the OAM.
*
*/
void updateOAM( OAMTable * p_oam );
void updateOAMSub( OAMTable * p_oam );

/*
*  initOAM
*
*  Initialize the OAM.
*
*/
void initOAMTable( OAMTable * p_oam );
void initOAMTableSub( OAMTable * p_oam );

/*
*  rotateSprite
*
*  Rotate a sprite.
*
*/
void rotateSprite( SpriteRotation * p_spriteRotation, u16 p_angle );

/*
*  setSpriteVisibility
*
*  Hide or show a sprite of a certain type: affine double bound, affine
*  non-double bound, or ordinary.
*
*/
void setSpriteVisibility( SpriteEntry * p_spriteEntry, bool p_hidden, bool p_affine = false, bool p_doubleBound = false );

inline void setSpritePosition( SpriteEntry* p_spriteEntry, u16 p_x = 0, u16 p_y = 0 );
inline void setSpritePriority( SpriteEntry* p_spriteEntry, ObjPriority p_priority );

u16 loadSprite( OAMTable    *p_oam,
                SpriteInfo  *p_spriteInfo,
                const u8    p_oamIdx,
                const u8    p_palIdx,
                const u16   p_tileIdx,
                const u16   p_posX,
                const u16   p_posY,
                const u8    p_width,
                const u8    p_height,
                const unsigned short *p_spritePal,
                const unsigned int   *p_spriteData,
                const u32   p_spriteDataLen,
                bool        p_flipX,
                bool        p_flipY,
                bool        p_hidden,
                ObjPriority p_priority,
                bool        p_subScreen );
//Some specific things
u16 initMainSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo );
