/*
Pokémon Emerald 2 Version
------------------------------

file        : sprite.cpp
author      : Philip Wellnitz (RedArceus)
description : Some sprite code.

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

#include <algorithm>

#include "sprite.h"

#include "damage_0.h"
#include "damage_1.h"
#include "damage_2.h"

namespace IO {
    const unsigned int* TypeTiles[ 19 ] =
    {
        NormalTiles, KampfTiles, FlugTiles, GiftTiles, BodenTiles,
        GestTiles, KaeferTiles, GeistTiles, StahlTiles, UnbekTiles,
        WasserTiles, FeuerTiles, PflTiles, ElekTiles, PsychoTiles,
        EisTiles, DraTiles, UnlTiles, FeeTiles
    };
    const unsigned short* TypePals[ 19 ] =
    {
        NormalPal, KampfPal, FlugPal, GiftPal, BodenPal,
        GestPal, KaeferPal, GeistPal, StahlPal, UnbekPal,
        WasserPal, FeuerPal, PflPal, ElekPal, PsychoPal,
        EisPal, DraPal, UnlPal, FeePal
    };

    const unsigned int* HitTypeTiles[ 3 ] =
    {
        damage_0Tiles, damage_1Tiles, damage_2Tiles
    };
    const unsigned short* HitTypePals[ 3 ] =
    {
        damage_0Pal, damage_1Pal, damage_2Pal
    };

    void updateOAM( OAMTable * p_oam ) {
        DC_FlushAll( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          p_oam->oamBuffer,
                          OAM,
                          SPRITE_COUNT * sizeof( SpriteEntry ) );
    }
    void updateOAMSub( OAMTable * p_oam ) {
        DC_FlushAll( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          p_oam->oamBuffer,
                          OAM_SUB,
                          SPRITE_COUNT * sizeof( SpriteEntry ) );
    }

    void initOAMTable( OAMTable * p_oam ) {
        /*
        * For all 128 sprites on the DS, disable and clear any attributes they
        * might have. This prevents any garbage from being displayed and gives
        * us a clean slate to work with.
        */
        for( int i = 0; i < SPRITE_COUNT; i++ ) {
            p_oam->oamBuffer[ i ].attribute[ 0 ] = ATTR0_DISABLED;
            p_oam->oamBuffer[ i ].attribute[ 1 ] = 0;
            p_oam->oamBuffer[ i ].attribute[ 2 ] = 0;
            p_oam->oamBuffer[ i ].isHidden = true;
        }
        for( int i = 0; i < MATRIX_COUNT; i++ ) {
            /* If you look carefully, you'll see this is that affine trasformation
            * matrix again. We initialize it to the identity matrix, as we did
            * with backgrounds
            */
            p_oam->matrixBuffer[ i ].hdx = 1 << 8;
            p_oam->matrixBuffer[ i ].hdy = 0;
            p_oam->matrixBuffer[ i ].vdx = 0;
            p_oam->matrixBuffer[ i ].vdy = 1 << 8;
        }
        updateOAM( p_oam );
    }
    void initOAMTableSub( OAMTable * p_oam ) {
        /*
        * For all 128 sprites on the DS, disable and clear any attributes they
        * might have. This prevents any garbage from being displayed and gives
        * us a clean slate to work with.
        */
        for( int i = 0; i < SPRITE_COUNT; i++ ) {
            p_oam->oamBuffer[ i ].attribute[ 0 ] = ATTR0_DISABLED;
            p_oam->oamBuffer[ i ].attribute[ 1 ] = 0;
            p_oam->oamBuffer[ i ].attribute[ 2 ] = 0;
            p_oam->oamBuffer[ i ].isHidden = true;
        }
        for( int i = 0; i < MATRIX_COUNT; i++ ) {
            /* If you look carefully, you'll see this is that affine trasformation
            * matrix again. We initialize it to the identity matrix, as we did
            * with backgrounds
            */
            p_oam->matrixBuffer[ i ].hdx = 1 << 8;
            p_oam->matrixBuffer[ i ].hdy = 0;
            p_oam->matrixBuffer[ i ].vdx = 0;
            p_oam->matrixBuffer[ i ].vdy = 1 << 8;
        }

        updateOAMSub( p_oam );
    }
    void rotateSprite( SpriteRotation * p_spriteRotation, int p_angle ) {
        s16 s = sinLerp( p_angle ) >> 4;
        s16 c = cosLerp( p_angle ) >> 4;

        p_spriteRotation->hdx = c;
        p_spriteRotation->hdy = s;
        p_spriteRotation->vdx = -s;
        p_spriteRotation->vdy = c;
    }
    void setSpriteVisibility( SpriteEntry * p_spriteEntry, bool p_hidden, bool p_affine, bool p_doubleBound ) {
        if( p_hidden ) {
            /*
            * Make the sprite invisible.
            *
            * An affine sprite cannot be hidden. We have to turn it into a
            * non-affine sprite before we can hide it. To hide any sprite, we must
            * set bit 8 and clear bit 9. For non-affine sprites, this is a bit
            * redundant, but it is faster than a branch to just set it regardless
            * of whether or not it is already set.
            */
            p_spriteEntry->isRotateScale = false; // Bit 9 off
            p_spriteEntry->isHidden = true; // Bit 8 on
        } else {
            /* Make the sprite visible.*/
            if( p_affine ) {
                /* Again, keep in mind that affine sprites cannot be hidden, so
                * enabling affine is enough to show the sprite again. We also need
                * to allow the user to get the double bound flag in the sprite
                * attribute. If we did not, then our sprite hiding function would
                * not be able to properly hide and restore double bound sprites.
                * We enable bit 9 here because we want an affine sprite.
                */
                p_spriteEntry->isRotateScale = true;

                /* The double bound flag only acts as the double bound flag when
                * the sprite is an affine sprite. At all other times, it acts as
                * the sprite invisibility flag. We only enable bit 8 here if we want
                * a double bound sprite. */
                p_spriteEntry->isSizeDouble = p_doubleBound;
            } else {
                /* Bit 9 (the affine flag) will already be off here, so we don't
                * need to clear it. However, bit 8 (the sprite invisibility flag)
                * will need to be cleared. */
                p_spriteEntry->isHidden = false;
            }
        }
    }
    inline void setSpritePosition( SpriteEntry* p_spriteEntry, u16 p_x, u16 p_y ) {
        p_spriteEntry->x = p_x;
        p_spriteEntry->y = p_y;
    }
    inline void setSpritePriority( SpriteEntry* p_spriteEntry, ObjPriority p_priority ) {
        p_spriteEntry->priority = p_priority;
    }

    u16 loadSprite( OAMTable   *p_oam,
                    SpriteInfo *p_spriteInfo,
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
                    bool        p_subScreen ) {

        SpriteInfo * spriteInfo = &p_spriteInfo[ p_oamIdx ];
        SpriteEntry * spriteEntry = &p_oam->oamBuffer[ p_oamIdx ];

        spriteInfo->m_oamId = p_oamIdx;
        spriteInfo->m_width = p_width;
        spriteInfo->m_height = p_height;
        spriteInfo->m_angle = 0;
        spriteInfo->m_entry = spriteEntry;

        spriteEntry->palette = p_palIdx;
        spriteEntry->gfxIndex = p_tileIdx;
        spriteEntry->x = p_posX;
        spriteEntry->y = p_posY;
        spriteEntry->vFlip = p_flipX;
        spriteEntry->hFlip = p_flipY;
        spriteEntry->isHidden = p_hidden;
        spriteEntry->priority = p_priority;

        spriteEntry->isRotateScale = false;
        spriteEntry->isMosaic = false;
        spriteEntry->blendMode = OBJMODE_NORMAL;
        spriteEntry->colorMode = OBJCOLOR_16;

        spriteEntry->shape = ( ( p_width == p_height ) ? OBJSHAPE_SQUARE : ( ( p_width > p_height ) ? OBJSHAPE_WIDE : OBJSHAPE_TALL ) );

        u8 maxSize = std::max( p_width, p_height );
        spriteEntry->size = ( ( maxSize == 64 ) ? OBJSIZE_64 :
                              ( ( maxSize == 32 ) ? OBJSIZE_32 :
                              ( ( maxSize == 16 ) ? OBJSIZE_16 : OBJSIZE_8 ) ) );

        if( !p_subScreen ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spritePal, &SPRITE_PALETTE[ p_palIdx * COLORS_PER_PALETTE ], 32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spriteData, &SPRITE_GFX[ p_tileIdx * OFFSET_MULTIPLIER ], p_spriteDataLen );
        } else {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spritePal, &SPRITE_PALETTE_SUB[ p_palIdx * COLORS_PER_PALETTE ], 32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spriteData, &SPRITE_GFX_SUB[ p_tileIdx * OFFSET_MULTIPLIER_SUB ], p_spriteDataLen );
        }
        return p_tileIdx + ( p_spriteDataLen / BYTES_PER_16_COLOR_TILE );
    }
}