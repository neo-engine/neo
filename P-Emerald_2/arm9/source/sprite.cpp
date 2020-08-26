/*
Pokémon neo
------------------------------

file        : sprite.cpp
author      : Philip Wellnitz
description : Some sprite code.

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <cstdio>
#include <initializer_list>

#include "fs.h"
#include "pokemonNames.h"
#include "sprite.h"
#include "uio.h"

#include "NoItem.h"
#include "damage_0.h"
#include "damage_1.h"
#include "damage_2.h"

#include "type_bug_de.h"
#include "type_bug_en.h"
#include "type_dark_de.h"
#include "type_dark_en.h"
#include "type_dragon_de.h"
#include "type_dragon_en.h"
#include "type_electr_de.h"
#include "type_electr_en.h"
#include "type_fairy_de.h"
#include "type_fairy_en.h"
#include "type_fight_de.h"
#include "type_fight_en.h"
#include "type_fire_de.h"
#include "type_fire_en.h"
#include "type_flying_de.h"
#include "type_flying_en.h"
#include "type_ghost_de.h"
#include "type_ghost_en.h"
#include "type_grass_de.h"
#include "type_grass_en.h"
#include "type_ground_de.h"
#include "type_ground_en.h"
#include "type_ice_de.h"
#include "type_ice_en.h"
#include "type_normal_de.h"
#include "type_normal_en.h"
#include "type_poison_de.h"
#include "type_poison_en.h"
#include "type_psychic_de.h"
#include "type_psychic_en.h"
#include "type_rock_de.h"
#include "type_rock_en.h"
#include "type_steel_de.h"
#include "type_steel_en.h"
#include "type_unknown.h"
#include "type_water_de.h"
#include "type_water_en.h"

unsigned int   TEMP[ 12288 ]   = { 0 };
unsigned short TEMP_PAL[ 256 ] = { 0 };

namespace IO {
    const char* OW_PATH        = "nitro:/PICS/SPRITES/OW/";
    const char* OWP_PATH       = "nitro:/PICS/SPRITES/NPCP/";
    const char* TRAINER_PATH   = "nitro:/PICS/SPRITES/NPC/";
    const char* BERRY_PATH   = "nitro:/PICS/SPRITES/BERRIES/";
    const char* TM_PATH        = "nitro:/PICS/SPRITES/TM/";
    const char* ITEM_PATH      = "nitro:/PICS/SPRITES/ITEMS/";
    const char* PKMN_PATH      = "nitro:/PICS/SPRITES/PKMN/";
    const char* PKMN_BACK_PATH = "nitro:/PICS/SPRITES/PKMNBACK/";
    const char* ICON_PATH      = "nitro:/PICS/SPRITES/ICONS/";

    const unsigned int* TypeTiles[ 19 ][ LANGUAGES ]
        = { { type_normal_enTiles, type_normal_deTiles },
            { type_fight_enTiles, type_fight_deTiles },
            { type_flying_enTiles, type_flying_deTiles },
            { type_poison_enTiles, type_poison_deTiles },
            { type_ground_enTiles, type_ground_deTiles },
            { type_rock_enTiles, type_rock_deTiles },
            { type_bug_enTiles, type_bug_deTiles },
            { type_ghost_enTiles, type_ghost_deTiles },
            { type_steel_enTiles, type_steel_deTiles },
            { type_unknownTiles, type_unknownTiles },
            { type_water_enTiles, type_water_deTiles },
            { type_fire_enTiles, type_fire_deTiles },
            { type_grass_enTiles, type_grass_deTiles },
            { type_electr_enTiles, type_electr_deTiles },
            { type_psychic_enTiles, type_psychic_deTiles },
            { type_ice_enTiles, type_ice_deTiles },
            { type_dragon_enTiles, type_dragon_deTiles },
            { type_dark_enTiles, type_dark_deTiles },
            { type_fairy_enTiles, type_fairy_deTiles } };
    const unsigned short* TypePals[ 19 ][ LANGUAGES ]
        = { { type_normal_enPal, type_normal_dePal },   { type_fight_enPal, type_fight_dePal },
            { type_flying_enPal, type_flying_dePal },   { type_poison_enPal, type_poison_dePal },
            { type_ground_enPal, type_ground_dePal },   { type_rock_enPal, type_rock_dePal },
            { type_bug_enPal, type_bug_dePal },         { type_ghost_enPal, type_ghost_dePal },
            { type_steel_enPal, type_steel_dePal },     { type_unknownPal, type_unknownPal },
            { type_water_enPal, type_water_dePal },     { type_fire_enPal, type_fire_dePal },
            { type_grass_enPal, type_grass_dePal },     { type_electr_enPal, type_electr_dePal },
            { type_psychic_enPal, type_psychic_dePal }, { type_ice_enPal, type_ice_dePal },
            { type_dragon_enPal, type_dragon_dePal },   { type_dark_enPal, type_dark_dePal },
            { type_fairy_enPal, type_fairy_dePal } };

    const unsigned int*   HitTypeTiles[ 3 ] = { damage_0Tiles, damage_1Tiles, damage_2Tiles };
    const unsigned short* HitTypePals[ 3 ]  = { damage_0Pal, damage_1Pal, damage_2Pal };

    const u8  SPRITE_DMA_CHANNEL      = 2;
    const u16 BYTES_PER_16_COLOR_TILE = 32;
    const u16 COLORS_PER_PALETTE      = 16;
    const u16 BOUNDARY_VALUE          = 32;
    const u16 OFFSET_MULTIPLIER       = ( BOUNDARY_VALUE / sizeof( SPRITE_GFX[ 0 ] ) );
    const u16 OFFSET_MULTIPLIER_SUB   = ( BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] ) );

    // Lower-y of bounding box of sprite
    // (identify -format "%[bounding-box]" $i.png | string match -r '\d+$')
    constexpr u16 SPECIES_HEIGHTS[ MAX_PKMN + 5 ] = {
        79, 63, 74, 81, 71, 78, 88, 68, 78, 80, 68, 66, 83, 67, 68, 88, 68, 90, 82, 65, 73, 66, 90,
        69, 79, 70, 77, 66, 71, 68, 67, 82, 64, 71, 84, 70, 76, 74, 84, 65, 76, 75, 87, 67, 71, 74,
        64, 77, 73, 84, 64, 69, 72, 80, 69, 78, 73, 76, 68, 84, 69, 72, 77, 70, 78, 82, 73, 82, 83,
        70, 78, 85, 75, 82, 59, 70, 79, 77, 86, 71, 80, 70, 83, 74, 74, 87, 72, 83, 66, 78, 64, 77,
        86, 79, 76, 84, 74, 77, 69, 78, 63, 71, 66, 85, 66, 69, 80, 77, 76, 84, 90, 75, 81, 70, 70,
        80, 66, 72, 66, 73, 69, 73, 74, 80, 73, 77, 79, 76, 77, 76, 96, 81, 62, 68, 74, 71, 73, 68,
        65, 72, 63, 77, 86, 82, 87, 88, 93, 70, 79, 87, 83, 79, 72, 73, 89, 65, 71, 83, 66, 76, 84,
        78, 76, 69, 80, 72, 79, 62, 72, 78, 66, 72, 63, 62, 64, 62, 74, 65, 74, 71, 73, 80, 65, 65,
        75, 72, 74, 78, 74, 79, 77, 63, 72, 79, 65, 77, 75, 73, 68, 82, 79, 75, 74, 82, 67, 72, 68,
        72, 86, 67, 75, 72, 87, 70, 75, 72, 66, 83, 69, 76, 61, 77, 69, 66, 70, 74, 85, 82, 69, 80,
        81, 65, 72, 65, 84, 72, 70, 73, 65, 68, 67, 72, 75, 83, 82, 82, 72, 70, 86, 96, 96, 76, 71,
        77, 83, 70, 79, 86, 69, 76, 77, 68, 80, 71, 65, 68, 70, 83, 67, 79, 65, 71, 84, 67, 76, 81,
        66, 76, 78, 89, 67, 75, 84, 69, 87, 64, 79, 63, 75, 76, 61, 81, 80, 66, 78, 81, 74, 86, 68,
        71, 67, 76, 70, 71, 63, 72, 84, 69, 81, 66, 82, 69, 69, 73, 70, 71, 67, 78, 73, 82, 71, 85,
        72, 79, 80, 74, 80, 72, 69, 71, 93, 67, 83, 76, 89, 79, 77, 81, 90, 67, 70, 70, 78, 72, 85,
        75, 87, 72, 81, 71, 86, 74, 75, 77, 83, 79, 82, 88, 86, 77, 68, 67, 86, 65, 71, 81, 69, 81,
        73, 76, 65, 72, 72, 83, 75, 85, 74, 83, 81, 89, 84, 84, 76, 85, 89, 78, 86, 69, 73, 88, 72,
        72, 80, 63, 75, 86, 67, 73, 76, 68, 76, 71, 77, 66, 72, 79, 70, 77, 76, 87, 67, 77, 80, 83,
        80, 75, 87, 73, 73, 82, 64, 71, 65, 75, 83, 87, 88, 76, 80, 90, 76, 77, 77, 77, 69, 75, 77,
        87, 73, 78, 69, 69, 74, 71, 79, 86, 71, 70, 80, 68, 79, 71, 79, 69, 77, 88, 69, 82, 77, 74,
        83, 81, 81, 81, 83, 80, 84, 83, 69, 84, 73, 75, 82, 82, 79, 83, 84, 86, 80, 83, 77, 83, 73,
        92, 82, 76, 82, 87, 92, 66, 67, 86, 64, 84, 75, 67, 77, 82, 64, 72, 85, 66, 76, 88, 70, 82,
        70, 74, 82, 72, 79, 69, 80, 70, 76, 71, 77, 73, 88, 69, 72, 83, 78, 82, 66, 70, 84, 75, 82,
        73, 77, 71, 68, 72, 80, 65, 72, 83, 72, 76, 64, 66, 88, 64, 69, 89, 72, 83, 70, 81, 65, 61,
        79, 85, 66, 78, 84, 64, 79, 69, 79, 96, 76, 96, 63, 78, 68, 95, 66, 82, 71, 82, 75, 78, 66,
        73, 85, 73, 79, 84, 69, 80, 74, 87, 91, 71, 86, 74, 64, 89, 64, 72, 88, 91, 93, 64, 71, 66,
        68, 77, 83, 87, 67, 79, 79, 81, 89, 66, 77, 90, 68, 76, 92, 65, 84, 93, 63, 73, 69, 72, 80,
        82, 77, 88, 71, 83, 77, 72, 96, 72, 80, 78, 64, 69, 78, 86, 70, 91, 86, 78, 83, 92, 92, 89,
        94, 92, 76, 78, 79, 81, 70, 72, 74, 69, 81, 90, 65, 72, 80, 77, 81, 66, 83, 96, 69, 65, 91,
        71, 84, 78, 82, 84, 73, 80, 69, 87, 80, 69, 75, 85, 85, 93, 76, 76, 75, 73, 70, 87, 80, 89,
        80, 85, 65, 87, 66, 83, 71, 89, 78, 88, 77, 78, 66, 77, 65, 77, 88, 96, 87, 88, 77, 93, 69,
        79, 86, 89, 96, 90, 92, 85, 92, 90, 65, 73, 90, 68, 73, 85, 67, 73, 84, 69, 73, 77, 73, 78,
        60, 64, 79, 79, 86, 75, 78, 83, 71, 78, 80, 71, 83, 79, 81, 75, 78, 70, 82, 71, 79, 72, 83,
        66, 82, 66, 75, 86, 81, 83, 78, 71, 89, 71, 76, 61, 87, 94, 79, 70, 84, 68, 72, 66, 78, 90,
        73, 82, 88, 86, 87, 89, 85, 81, 84, 92, 96, 93, 95, 94, 96, 96, 88, 90, 90, 81, 82, 75, 93,
        94, 93, 90, 71, 75, 68, 76, 88, 71, 77, 88, 69, 73, 88, 72, 85, 68, 91, 82, 70, 69, 92, 74,
        80, 69, 77, 70, 85, 71, 72, 69, 84, 68, 77, 89, 67, 77, 83, 68, 75, 84, 65, 66, 73, 90, 67,
        84, 66, 80, 72, 88, 66, 74, 87, 73, 78, 87, 88, 78, 91, 95, 76, 86, 65, 0,  74, 68, 62, 91,
        86, 89, 74, 73, 74, 83, 84, 88, 89, 80, 85, 78, 89, 87, 95, 92, 96,
    };

    void updateOAM( bool p_bottom ) {
        OAMTable* oam = ( p_bottom ? Oam : OamTop );
        DC_FlushAll( );
        if( p_bottom ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, oam->oamBuffer, OAM_SUB,
                              SPRITE_COUNT * sizeof( SpriteEntry ) );
        } else {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, oam->oamBuffer, OAM,
                              SPRITE_COUNT * sizeof( SpriteEntry ) );
        }
    }
    void initOAMTable( bool p_bottom ) {
        OAMTable* oam = ( p_bottom ? Oam : OamTop );
        /*
         * For all 128 sprites on the DS, disable and clear any attributes they
         * might have. This prevents any garbage from being displayed and gives
         * us a clean slate to work with.
         */
        for( int i = 0; i < SPRITE_COUNT; i++ ) {
            oam->oamBuffer[ i ].attribute[ 0 ] = ATTR0_DISABLED;
            oam->oamBuffer[ i ].attribute[ 1 ] = 0;
            oam->oamBuffer[ i ].attribute[ 2 ] = 0;
            oam->oamBuffer[ i ].isHidden       = true;
        }
        for( int i = 0; i < MATRIX_COUNT; i++ ) {
            /* If you look carefully, you'll see this is that affine trasformation
             * matrix again. We initialize it to the identity matrix, as we did
             * with backgrounds
             */
            oam->matrixBuffer[ i ].hdx = 1 << 8;
            oam->matrixBuffer[ i ].hdy = 0;
            oam->matrixBuffer[ i ].vdx = 0;
            oam->matrixBuffer[ i ].vdy = 1 << 8;
        }

        if( p_bottom )
            memset( SPRITE_GFX_SUB, 0, 1024 );
        else
            memset( SPRITE_GFX, 0, 1024 );
        updateOAM( p_bottom );
    }
    void rotateSprite( SpriteRotation* p_spriteRotation, int p_angle ) {
        s16 s = sinLerp( p_angle ) >> 4;
        s16 c = cosLerp( p_angle ) >> 4;

        p_spriteRotation->hdx = c;
        p_spriteRotation->hdy = s;
        p_spriteRotation->vdx = -s;
        p_spriteRotation->vdy = c;
    }
    void setSpriteVisibility( SpriteEntry* p_spriteEntry, bool p_hidden, bool p_affine,
                              bool p_doubleBound ) {
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
            p_spriteEntry->isHidden      = true;  // Bit 8 on
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

    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, bool p_bottom ) {
        copySpritePal( p_spritePal, p_palIdx, 32, p_bottom );
    }
    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, const u16 p_palLen,
                        bool p_bottom ) {
        copySpritePal( p_spritePal, p_palIdx, 0, p_palLen, p_bottom );
    }
    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, const u8 p_startIdx,
                        const u16 p_palLen, bool p_bottom ) {
        if( !p_bottom && p_spritePal )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spritePal,
                              &SPRITE_PALETTE[ p_palIdx * COLORS_PER_PALETTE + p_startIdx ],
                              p_palLen );
        else if( p_spritePal )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spritePal,
                              &SPRITE_PALETTE_SUB[ p_palIdx * COLORS_PER_PALETTE + p_startIdx ],
                              p_palLen );
    }
    void copySpriteData( const unsigned int* p_spriteData, const u16 p_tileCnt,
                         const u32 p_spriteDataLen, bool p_bottom ) {
        if( !p_bottom && p_spriteData )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spriteData,
                              &SPRITE_GFX[ (u32) p_tileCnt * OFFSET_MULTIPLIER ], p_spriteDataLen );
        else if( p_spriteData )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spriteData,
                              &SPRITE_GFX_SUB[ (u32) p_tileCnt * OFFSET_MULTIPLIER_SUB ],
                              p_spriteDataLen );
    }

    u16 loadSprite( const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileCnt, const u16 p_posX,
                    const u16 p_posY, const u8 p_width, const u8 p_height,
                    const unsigned short* p_spritePal, const unsigned int* p_spriteData,
                    const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                    ObjPriority p_priority, bool p_bottom, ObjBlendMode p_blendMode ) {
        IO::SpriteInfo* sInfo = ( p_bottom ? spriteInfo : spriteInfoTop ) + p_oamIdx;
        OAMTable*       oam   = ( p_bottom ? Oam : OamTop );

        SpriteEntry* spriteEntry = &oam->oamBuffer[ p_oamIdx ];

        sInfo->m_oamId  = p_oamIdx;
        sInfo->m_width  = p_width;
        sInfo->m_height = p_height;
        sInfo->m_angle  = 0;
        sInfo->m_entry  = spriteEntry;

        spriteEntry->palette  = p_palIdx;
        spriteEntry->gfxIndex = p_tileCnt;
        spriteEntry->x        = p_posX;
        spriteEntry->y        = p_posY;
        spriteEntry->vFlip    = p_flipX;
        spriteEntry->hFlip    = p_flipY;
        spriteEntry->isHidden = p_hidden;
        spriteEntry->priority = p_priority;

        spriteEntry->isRotateScale = false;
        spriteEntry->isMosaic      = false;
        spriteEntry->blendMode     = p_blendMode;
        spriteEntry->colorMode     = OBJCOLOR_16;

        spriteEntry->shape = ( ( p_width == p_height )
                                   ? OBJSHAPE_SQUARE
                                   : ( ( p_width > p_height ) ? OBJSHAPE_WIDE : OBJSHAPE_TALL ) );

        u8 maxSize = std::max( p_width, p_height );
        spriteEntry->size
            = ( ( maxSize == 64 )
                    ? OBJSIZE_64
                    : ( ( maxSize == 32 ) ? OBJSIZE_32
                                          : ( ( maxSize == 16 ) ? OBJSIZE_16 : OBJSIZE_8 ) ) );

        copySpriteData( p_spriteData, p_tileCnt, p_spriteDataLen, p_bottom );
        copySpritePal( p_spritePal, p_palIdx, p_bottom );
        return p_tileCnt + ( p_spriteDataLen / BYTES_PER_16_COLOR_TILE );
    }

    u16 loadSprite( const char* p_name, const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileCnt,
                    const u16 p_posX, const u16 p_posY, const u8 p_width, const u8 p_height,
                    bool p_flipX, bool p_flipY, bool p_hidden, ObjPriority p_priority,
                    bool p_bottom, ObjBlendMode p_blendMode ) {
        if( FS::readData( ICON_PATH, p_name, (unsigned int) p_width * p_height / 8, TEMP,
                          (unsigned short) 16, TEMP_PAL ) ) {
            return loadSprite( p_oamIdx, p_palIdx, p_tileCnt, p_posX, p_posY, p_width, p_height,
                               TEMP_PAL, TEMP, p_width * p_height / 2, p_flipX, p_flipY, p_hidden,
                               p_priority, p_bottom, p_blendMode );
        }
        return p_tileCnt + ( p_width * p_height / 2 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 BITMAP_SPRITE[ 64 * 64 * 2 ];
    u16 loadSpriteB( const u8 p_oamIdx, const u16 p_tileCnt, const u16 p_posX, const u16 p_posY,
                     const u8 p_width, const u8 p_height, const unsigned short* p_spritePal,
                     const unsigned int* p_spriteData, const u32 p_spriteDataLen, bool p_flipX,
                     bool p_flipY, bool p_hidden, ObjPriority p_priority, bool p_bottom,
                     bool p_outline, u16 p_outlineColor, bool p_tiled ) {
        memset( BITMAP_SPRITE, 0, sizeof( BITMAP_SPRITE ) );
        if( p_spritePal && p_spriteData && p_tiled ) {
            u32 i = 0;
            for( u8 tiley = 0; tiley < p_height / 8; ++tiley ) {
                for( u8 tilex = 0; tilex < p_width / 8; ++tilex ) {
                    int shift = tilex * ( -28 ); // size per tile
                    for( u8 y = 0; y < 8; ++y ) {
                        for( u8 x = 0; x < 8; x += 2, ++i ) {
                            u8 cur = reinterpret_cast<const u8*>( p_spriteData )[ i ];
                            u8 up = cur >> 4, down = cur & 0xf;
                            BITMAP_SPRITE[ 2 * ( i + shift ) + 1 ]
                                = ( !!up ) * ( ( 1 << 15 ) | p_spritePal[ up ] );
                            BITMAP_SPRITE[ 2 * ( i + shift ) ]
                                = ( !!down ) * ( ( 1 << 15 ) | p_spritePal[ down ] );
                        }
                        shift += 4 * ( p_width / 8 - 1 );
                    }
                }
            }
        } else if( p_spritePal && p_spriteData ) {
            u32 i = 0;
            for( u8 y = 0; y < p_height; ++y ) {
                for( u8 x = 0; x < p_width; x += 2, ++i ) {
                    u8 cur = reinterpret_cast<const u8*>( p_spriteData )[ i ];
                    u8 up = cur >> 4, down = cur & 0xf;
                    BITMAP_SPRITE[ 2 * ( i ) + 1 ] = ( !!up ) * ( ( 1 << 15 ) | p_spritePal[ up ] );
                    BITMAP_SPRITE[ 2 * ( i ) ] = ( !!down ) * ( ( 1 << 15 ) | p_spritePal[ down ] );
                }
            }
        }

        if( p_outline ) {
            for( u8 y = 0; y < p_height; ++y ) {
                for( u8 x = 0; x < p_width; ++x ) {
                    if( !( BITMAP_SPRITE[ p_width * y + x ] & ( 1 << 15 ) ) ) {
                        bool isOutline = false;
                        for( s8 dx = -1; dx < 2; ++dx ) {
                            for( s8 dy = -1; dy < 2; ++dy ) {
                                if( x + dx >= 0 && x + dx < p_width && y + dy >= 0
                                    && y + dy < p_height ) {
                                    auto cur = BITMAP_SPRITE[ p_width * ( y + dy ) + x + dx ];
                                    if( ( cur & ( 1 << 15 ) ) && cur != p_outlineColor ) {
                                        isOutline = true;
                                    }
                                }
                            }
                        }
                        if( isOutline ) { BITMAP_SPRITE[ p_width * y + x ] = p_outlineColor; }
                    }
                }
            }
        }

        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, p_width, p_height,
                            ( p_spritePal && p_spriteData ) ? BITMAP_SPRITE : 0, p_spriteDataLen,
                            p_flipX, p_flipY, p_hidden, p_priority, p_bottom );
    }

    u16 loadSpriteB( const u8 p_oamIdx, const u16 p_tileCnt, const u16 p_posX, const u16 p_posY,
                     const u8 p_width, const u8 p_height, const unsigned short* p_spriteData,
                     const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                     ObjPriority p_priority, bool p_bottom ) {
        IO::SpriteInfo* sInfo = ( p_bottom ? spriteInfo : spriteInfoTop ) + p_oamIdx;
        OAMTable*       oam   = ( p_bottom ? Oam : OamTop );

        SpriteEntry* spriteEntry = &oam->oamBuffer[ p_oamIdx ];

        sInfo->m_oamId  = p_oamIdx;
        sInfo->m_width  = p_width;
        sInfo->m_height = p_height;
        sInfo->m_angle  = 0;
        sInfo->m_entry  = spriteEntry;

        spriteEntry->palette  = 1;
        spriteEntry->gfxIndex = p_tileCnt;
        spriteEntry->x        = p_posX;
        spriteEntry->y        = p_posY;
        spriteEntry->vFlip    = p_flipX;
        spriteEntry->hFlip    = p_flipY;
        spriteEntry->isHidden = p_hidden;
        spriteEntry->priority = p_priority;

        spriteEntry->isRotateScale = false;
        spriteEntry->isMosaic      = false;
        spriteEntry->blendMode     = OBJMODE_BITMAP;
        spriteEntry->colorMode     = OBJCOLOR_256;

        spriteEntry->shape = ( ( p_width == p_height )
                                   ? OBJSHAPE_SQUARE
                                   : ( ( p_width > p_height ) ? OBJSHAPE_WIDE : OBJSHAPE_TALL ) );

        u8 maxSize = std::max( p_width, p_height );
        spriteEntry->size
            = ( ( maxSize == 64 )
                    ? OBJSIZE_64
                    : ( ( maxSize == 32 ) ? OBJSIZE_32
                                          : ( ( maxSize == 16 ) ? OBJSIZE_16 : OBJSIZE_8 ) ) );

        auto gfx = p_bottom ? &SPRITE_GFX_SUB[ (u32) p_tileCnt * 64 ]
                            : &SPRITE_GFX[ (u32) p_tileCnt * 64 ];
        if( p_spriteData ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spriteData, gfx, p_width * p_height * 2 );
        }
        return p_tileCnt + ( p_spriteDataLen / BYTES_PER_16_COLOR_TILE );
    }

    u16 loadSpriteB( const char* p_name, const u8 p_oamIdx, const u16 p_tileCnt, const u16 p_posX,
                     const u16 p_posY, const u8 p_width, const u8 p_height, bool p_flipX,
                     bool p_flipY, bool p_hidden, ObjPriority p_priority, bool p_bottom,
                     bool p_outline, u16 p_outlineColor, bool p_tiled ) {
        if( FS::readData( ICON_PATH, p_name, (unsigned int) p_width * p_height / 8, TEMP,
                          (unsigned short) 16, TEMP_PAL ) ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, p_width, p_height, TEMP_PAL,
                                TEMP, p_width * p_height / 2, p_flipX, p_flipY, p_hidden,
                                p_priority, p_bottom, p_outline, p_outlineColor, p_tiled );
        }
        return p_tileCnt + ( p_width * p_height / 2 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 pkmnSpriteHeight( u16 p_speciesId ) {
        return 96 - SPECIES_HEIGHTS[ p_speciesId ];
    }

    char BUFFER[ 100 ];

    constexpr bool existsPKMNSprite( const u16 p_pkmnId, bool p_flipx, bool p_female ) {
        if( p_flipx ) {
            switch( p_pkmnId ) {
            case PKMN_UNOWN:
            case PKMN_ABSOL:
            case PKMN_AMOONGUSS:
            case PKMN_BARBARACLE:
            case PKMN_BLACEPHALON:
            case PKMN_BUDEW:
            case PKMN_CASTFORM:
            case PKMN_CLAUNCHER:
            case PKMN_CLAWITZER:
            case PKMN_CLEFABLE:
            case PKMN_CLEFAIRY:
            case PKMN_CLEFFA:
            case PKMN_COMFEY:
            case PKMN_CROCONAW:
            case PKMN_DARTRIX:
            case PKMN_DHELMISE:
            case PKMN_DRILBUR:
            case PKMN_DUGTRIO:
            case PKMN_EMBOAR:
            case PKMN_EXCADRILL:
            case PKMN_FURFROU:
            case PKMN_GARBODOR:
            case PKMN_GOLETT:
            case PKMN_GOLURK:
            case PKMN_GRENINJA:
            case PKMN_IGGLYBUFF:
            case PKMN_JIGGLYPUFF:
            case PKMN_KELDEO:
            case PKMN_KINGLER:
            case PKMN_KLANG:
            case PKMN_KLEFKI:
            case PKMN_KLINK:
            case PKMN_KLINKLANG:
            case PKMN_KYUREM:
            case PKMN_LILLIGANT:
            case PKMN_LITWICK:
            case PKMN_MAGMORTAR:
            case PKMN_MARSHADOW:
            case PKMN_MELOETTA:
            case PKMN_MINIOR:
            case PKMN_MUK:
            case PKMN_NECROZMA:
            case PKMN_PANSEAR:
            case PKMN_POLITOED:
            case PKMN_POLIWHIRL:
            case PKMN_POLIWRATH:
            case PKMN_REGIROCK:
            case PKMN_ROSELIA:
            case PKMN_ROSERADE:
            case PKMN_ROTOM:
            case PKMN_SAWK:
            case PKMN_SEVIPER:
            case PKMN_SHAYMIN:
            case PKMN_SIMISEAR:
            case PKMN_SNEASEL:
            case PKMN_SOLOSIS:
            case PKMN_STAKATAKA:
            case PKMN_STEENEE:
            case PKMN_SYLVEON:
            case PKMN_TEDDIURSA:
            case PKMN_TOGEKISS:
            case PKMN_TORTERRA:
            case PKMN_TSAREENA:
            case PKMN_VANILLUXE:
            case PKMN_WIGGLYTUFF:
            case PKMN_ZANGOOSE:
            case PKMN_ZYGARDE:
            case PKMN_VIVILLON: return false;
            default: return true;
            }
        }

        if( p_female ) {
            switch( p_pkmnId ) {
            case PKMN_VENUSAUR:
            case PKMN_BUTTERFREE:
            case PKMN_RATTATA:
            case PKMN_RATICATE:
            case PKMN_PIKACHU:
            case PKMN_RAICHU:
            case PKMN_ZUBAT:
            case PKMN_GOLBAT:
            case PKMN_GLOOM:
            case PKMN_VILEPLUME:
            case PKMN_KADABRA:
            case PKMN_ALAKAZAM:
            case PKMN_DODUO:
            case PKMN_DODRIO:
            case PKMN_HYPNO:
            case PKMN_RHYHORN:
            case PKMN_RHYDON:
            case PKMN_GOLDEEN:
            case PKMN_SEAKING:
            case PKMN_SCYTHER:
            case PKMN_MAGIKARP:
            case PKMN_GYARADOS:
            case PKMN_MEGANIUM:
            case PKMN_LEDYBA:
            case PKMN_LEDIAN:
            case PKMN_XATU:
            case PKMN_SUDOWOODO:
            case PKMN_POLITOED:
            case PKMN_AIPOM:
            case PKMN_WOOPER:
            case PKMN_QUAGSIRE:
            case PKMN_MURKROW:
            case PKMN_WOBBUFFET:
            case PKMN_GIRAFARIG:
            case PKMN_GLIGAR:
            case PKMN_STEELIX:
            case PKMN_SCIZOR:
            case PKMN_HERACROSS:
            case PKMN_SNEASEL:
            case PKMN_URSARING:
            case PKMN_PILOSWINE:
            case PKMN_OCTILLERY:
            case PKMN_HOUNDOOM:
            case PKMN_DONPHAN:
            case PKMN_TORCHIC:
            case PKMN_COMBUSKEN:
            case PKMN_BLAZIKEN:
            case PKMN_BEAUTIFLY:
            case PKMN_DUSTOX:
            case PKMN_LUDICOLO:
            case PKMN_NUZLEAF:
            case PKMN_SHIFTRY:
            case PKMN_MEDITITE:
            case PKMN_MEDICHAM:
            case PKMN_ROSELIA:
            case PKMN_GULPIN:
            case PKMN_SWALOT:
            case PKMN_NUMEL:
            case PKMN_CAMERUPT:
            case PKMN_CACTURNE:
            case PKMN_MILOTIC:
            case PKMN_RELICANTH:
            case PKMN_STARLY:
            case PKMN_STARAVIA:
            case PKMN_STARAPTOR:
            case PKMN_BIDOOF:
            case PKMN_BIBAREL:
            case PKMN_KRICKETOT:
            case PKMN_KRICKETUNE:
            case PKMN_SHINX:
            case PKMN_LUXIO:
            case PKMN_LUXRAY:
            case PKMN_ROSERADE:
            case PKMN_COMBEE:
            case PKMN_PACHIRISU:
            case PKMN_BUIZEL:
            case PKMN_FLOATZEL:
            case PKMN_AMBIPOM:
            case PKMN_GIBLE:
            case PKMN_GABITE:
            case PKMN_GARCHOMP:
            case PKMN_HIPPOPOTAS:
            case PKMN_HIPPOWDON:
            case PKMN_CROAGUNK:
            case PKMN_TOXICROAK:
            case PKMN_FINNEON:
            case PKMN_LUMINEON:
            case PKMN_SNOVER:
            case PKMN_ABOMASNOW:
            case PKMN_WEAVILE:
            case PKMN_RHYPERIOR:
            case PKMN_TANGROWTH:
            case PKMN_MAMOSWINE:
            case PKMN_UNFEZANT:
            case PKMN_FRILLISH:
            case PKMN_JELLICENT:
            case PKMN_PYROAR:
            case PKMN_MEOWSTIC:
            case PKMN_INDEEDEE: return true;
            default: return false;
            }
        }
        return true;
    }

    u16 loadPKMNSprite( const char* p_path, const u16 p_pkmnId, const s16 p_posX, const s16 p_posY,
                        u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom, bool p_shiny,
                        bool p_female, bool p_flipx, bool p_topOnly, u8 p_forme ) {

        memset( TEMP_PAL, 0, sizeof( TEMP_PAL ) );
        memset( TEMP, 0, sizeof( TEMP ) );
        if( !p_forme ) {
            snprintf( BUFFER, 99, "%02d/%d/%d%s%s", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId,
                      p_pkmnId, p_shiny ? "s" : "", p_female ? "f" : "" );
        } else {
            snprintf( BUFFER, 99, "%02d/%d/%d-%hhu%s%s", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId,
                      p_pkmnId, p_forme, p_shiny ? "s" : "", p_female ? "f" : "" );
        }
        if( !FS::readData<unsigned short, unsigned int>( p_path, BUFFER, 16, TEMP_PAL, 96 * 96 / 8,
                                                         TEMP ) ) {
            return false;
        }

        loadSprite( p_oamIdx++, p_palCnt, p_tileCnt, p_flipx ? 32 + p_posX : p_posX, p_posY, 64, 64,
                    TEMP_PAL, TEMP, 96 * 96 / 2, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        loadSprite( p_oamIdx++, p_palCnt, p_tileCnt + 64, p_flipx ? p_posX : 64 + p_posX, p_posY,
                    32, 64, 0, 0, 0, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        if( !p_topOnly ) {
            loadSprite( p_oamIdx++, p_palCnt, p_tileCnt + 96, p_flipx ? 32 + p_posX : p_posX,
                        p_posY + 64, 64, 32, 0, 0, 0, false, p_flipx, false, OBJPRIORITY_1,
                        p_bottom );
            loadSprite( p_oamIdx, p_palCnt, p_tileCnt + 128, p_flipx ? p_posX : 64 + p_posX,
                        p_posY + 64, 32, 32, 0, 0, 0, false, p_flipx, false, OBJPRIORITY_1,
                        p_bottom );
        }
        updateOAM( p_bottom );
        return p_tileCnt + 144;
    }
    u16 loadPKMNSprite( const u16 p_pkmnId, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                        u8 p_palCnt, u16 p_tileCnt, bool p_bottom, bool p_shiny, bool p_female,
                        bool p_flipx, bool p_topOnly, u8 p_forme ) {
        u16 res = 0;
        if( !existsPKMNSprite( p_pkmnId, true, false ) ) { p_flipx = false; }
        if( !existsPKMNSprite( p_pkmnId, false, true ) ) { p_female = false; }

        if( ( res
              = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                p_bottom, p_shiny, p_female, p_flipx, p_topOnly, p_forme ) ) ) {
            return res;
        }
        if( p_female
            && ( res = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, p_shiny, false, p_flipx, p_topOnly,
                                       p_forme ) ) ) {
            return res;
        }
        if( p_forme
            && ( res = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, p_shiny, p_female, p_flipx,
                                       p_topOnly ) ) ) {
            return res;
        }
        if( p_shiny
            && ( res = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, false, p_female, p_flipx, p_topOnly,
                                       p_forme ) ) ) {
            return res;
        }

        if( p_female && p_forme
            && ( res
                 = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                   p_tileCnt, p_bottom, p_shiny, false, p_flipx, p_topOnly ) ) ) {
            return res;
        }
        if( p_female && p_shiny
            && ( res = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, false, false, p_flipx, p_topOnly,
                                       p_forme ) ) ) {
            return res;
        }
        if( p_shiny && p_forme
            && ( res
                 = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                   p_tileCnt, p_bottom, false, p_female, p_flipx, p_topOnly ) ) ) {
            return res;
        }

        if( ( res = loadPKMNSprite( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                    p_tileCnt, p_bottom, false, false, p_flipx, p_topOnly ) ) ) {
            return res;
        }

        return loadPKMNSprite( PKMN_PATH, 0, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                               p_bottom, false, false, p_flipx, p_topOnly );
    }

    u16 loadPKMNSpriteB( const char* p_path, const u16 p_pkmnId, const s16 p_posX, const s16 p_posY,
                         u8 p_oamIdx, u16 p_tileCnt, bool p_bottom, bool p_shiny, bool p_female,
                         bool p_flipx, bool p_topOnly, u8 p_forme ) {

        memset( TEMP_PAL, 0, sizeof( TEMP_PAL ) );
        memset( TEMP, 0, sizeof( TEMP ) );
        if( !p_forme ) {
            snprintf( BUFFER, 99, "%02d/%d/%d%s%s", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId,
                      p_pkmnId, p_shiny ? "s" : "", p_female ? "f" : "" );
        } else {
            snprintf( BUFFER, 99, "%02d/%d/%d-%hhu%s%s", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId,
                      p_pkmnId, p_forme, p_shiny ? "s" : "", p_female ? "f" : "" );
        }
        if( !FS::readData<unsigned short, unsigned int>( p_path, BUFFER, 16, TEMP_PAL, 96 * 96 / 8,
                                                         TEMP ) ) {
            return false;
        }

        loadSpriteB( p_oamIdx++, p_tileCnt, p_flipx ? 32 + p_posX : p_posX, p_posY, 64, 64,
                     TEMP_PAL, TEMP, 64 * 64 / 2, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        loadSpriteB( p_oamIdx++, p_tileCnt + 64, p_flipx ? p_posX : 64 + p_posX, p_posY, 32, 64,
                     TEMP_PAL, TEMP + 64 * 64 / 8, 64 * 32 / 2, false, p_flipx, false,
                     OBJPRIORITY_1, p_bottom );
        if( !p_topOnly ) {
            loadSpriteB( p_oamIdx++, p_tileCnt + 96, p_flipx ? 32 + p_posX : p_posX, p_posY + 64,
                         64, 32, TEMP_PAL, TEMP + 64 * 64 / 8 + 32 * 64 / 8, 32 * 64 / 2, false,
                         p_flipx, false, OBJPRIORITY_1, p_bottom );
            loadSpriteB( p_oamIdx, p_tileCnt + 128, p_flipx ? p_posX : 64 + p_posX, p_posY + 64, 32,
                         32, TEMP_PAL, TEMP + 64 * 64 / 8 + 64 * 64 / 8, 32 * 32 / 8, false,
                         p_flipx, false, OBJPRIORITY_1, p_bottom );
        }
        updateOAM( p_bottom );
        return p_tileCnt + 144;
    }
    u16 loadPKMNSpriteB( const u16 p_pkmnId, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                         u16 p_tileCnt, bool p_bottom, bool p_shiny, bool p_female, bool p_flipx,
                         bool p_topOnly, u8 p_forme ) {
        u16 res = 0;
        if( !existsPKMNSprite( p_pkmnId, true, false ) ) { p_flipx = false; }
        if( !existsPKMNSprite( p_pkmnId, false, true ) ) { p_female = false; }

        if( ( res
              = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom,
                                 p_shiny, p_female, p_flipx, p_topOnly, p_forme ) ) ) {
            return res;
        }
        if( p_female
            && ( res
                 = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                    p_bottom, p_shiny, false, p_flipx, p_topOnly, p_forme ) ) ) {
            return res;
        }
        if( p_forme
            && ( res = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                        p_bottom, p_shiny, p_female, p_flipx, p_topOnly ) ) ) {
            return res;
        }
        if( p_shiny
            && ( res
                 = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                    p_bottom, false, p_female, p_flipx, p_topOnly, p_forme ) ) ) {
            return res;
        }

        if( p_female && p_forme
            && ( res = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                        p_bottom, p_shiny, false, p_flipx, p_topOnly ) ) ) {
            return res;
        }
        if( p_female && p_shiny
            && ( res = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                        p_bottom, false, false, p_flipx, p_topOnly, p_forme ) ) ) {
            return res;
        }
        if( p_shiny && p_forme
            && ( res = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                        p_bottom, false, p_female, p_flipx, p_topOnly ) ) ) {
            return res;
        }

        if( ( res = loadPKMNSpriteB( PKMN_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                     p_bottom, false, false, p_flipx, p_topOnly ) ) ) {
            return res;
        }

        return loadPKMNSpriteB( PKMN_PATH, 0, p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom, false,
                                false, p_flipx, p_topOnly );
    }

    u16 loadPKMNSpriteBack( const u16 p_pkmnId, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                            u8 p_palCnt, u16 p_tileCnt, bool p_bottom, bool p_shiny, bool p_female,
                            bool p_flipx, bool p_topOnly, u8 p_forme ) {
        u16 res = 0;
        if( !existsPKMNSprite( p_pkmnId, true, false ) ) { p_flipx = false; }
        if( !existsPKMNSprite( p_pkmnId, false, true ) ) { p_female = false; }

        if( ( res = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                    p_tileCnt, p_bottom, p_shiny, p_female, p_flipx, p_topOnly,
                                    p_forme ) ) ) {
            return res;
        }
        if( p_female
            && ( res = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, p_shiny, false, p_flipx, p_topOnly,
                                       p_forme ) ) ) {
            return res;
        }
        if( p_forme
            && ( res = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, p_shiny, p_female, p_flipx,
                                       p_topOnly ) ) ) {
            return res;
        }
        if( p_shiny
            && ( res = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, false, p_female, p_flipx, p_topOnly,
                                       p_forme ) ) ) {
            return res;
        }

        if( p_female && p_forme
            && ( res
                 = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                   p_tileCnt, p_bottom, p_shiny, false, p_flipx, p_topOnly ) ) ) {
            return res;
        }
        if( p_female && p_shiny
            && ( res = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                       p_tileCnt, p_bottom, false, false, p_flipx, p_topOnly,
                                       p_forme ) ) ) {
            return res;
        }
        if( p_shiny && p_forme
            && ( res
                 = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                   p_tileCnt, p_bottom, false, p_female, p_flipx, p_topOnly ) ) ) {
            return res;
        }

        if( ( res = loadPKMNSprite( PKMN_BACK_PATH, p_pkmnId, p_posX, p_posY, p_oamIdx, p_palCnt,
                                    p_tileCnt, p_bottom, false, false, p_flipx, p_topOnly ) ) ) {
            return res;
        }

        return loadPKMNSprite( PKMN_BACK_PATH, 0, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                               p_bottom, false, false, p_flipx, p_topOnly );
    }

    u16 loadEggSprite( const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                       bool p_bottom, bool p_manaphy ) {
        return loadPKMNSprite( 1 - 1, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom,
                               false, false, false, false, 1 + p_manaphy );
    }

    u16 loadTrainerSprite( u8 p_trainerId, const u16 p_posX, const u16 p_posY, u8 p_oamIdx,
                           u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {
        FILE* f = FS::openSplit( "nitro:/PICS/SPRITES/TRAINER/", p_trainerId, ".raw", 255 );
        if( !f ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal,
                               NoItemTiles, NoItemTilesLen, false, false, false, OBJPRIORITY_0,
                               p_bottom );
        }

        FS::read( f, TEMP, sizeof( u32 ), 512 );
        FS::read( f, TEMP_PAL, sizeof( u16 ), 16 );

        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 64, 64, TEMP_PAL, TEMP,
                           64 * 64 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadAnimatedSprite( FILE* p_file, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                            u8 p_palCnt, u16 p_tileCnt, ObjPriority p_priority, bool p_bottom ) {
        FS::read( p_file, TEMP_PAL, sizeof( u16 ), 16 );
        u8 frameCount, width, height;
        FS::read( p_file, &frameCount, sizeof( u8 ), 1 );
        FS::read( p_file, &width, sizeof( u8 ), 1 );
        FS::read( p_file, &height, sizeof( u8 ), 1 );
        FS::read( p_file, TEMP, sizeof( u32 ), width * height * frameCount / 8 );
        FS::close( p_file );

        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, width, height, TEMP_PAL,
                           TEMP, width * height * frameCount / 2, false, false, false, p_priority,
                           p_bottom );
    }
    u16 loadAnimatedSpriteB( FILE* p_file, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                             u16 p_tileCnt, ObjPriority p_priority, bool p_bottom, bool p_outline,
                             u16 p_outlineColor ) {
        FS::read( p_file, TEMP_PAL, sizeof( u16 ), 16 );
        u8 frameCount, width, height;
        FS::read( p_file, &frameCount, sizeof( u8 ), 1 );
        FS::read( p_file, &width, sizeof( u8 ), 1 );
        FS::read( p_file, &height, sizeof( u8 ), 1 );
        FS::read( p_file, TEMP, sizeof( u32 ), width * height * frameCount / 8 );
        FS::close( p_file );

        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, width, height, TEMP_PAL, TEMP,
                            width * height * frameCount / 2, false, false, false, p_priority,
                            p_bottom, p_outline, p_outlineColor );
    }

    u16 loadOWSprite( const u16 p_picnum, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                      u8 p_palCnt, u16 p_tileCnt ) {
        FILE* f = FS::open( OW_PATH, p_picnum, ".rsd" );
        return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, OBJPRIORITY_2,
                                   false );
    }

    u16 loadOWSpriteB( const u16 p_picnum, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                       u16 p_tileCnt, u16 p_palData[ 16 ], u32 p_dataBuffer[ 32 * 4 * 9 ] ) {
        FILE* f = FS::open( OW_PATH, p_picnum, ".rsd" );

        FS::read( f, p_palData, sizeof( u16 ), 16 );
        u8 frameCount, width, height;
        FS::read( f, &frameCount, sizeof( u8 ), 1 );
        FS::read( f, &width, sizeof( u8 ), 1 );
        FS::read( f, &height, sizeof( u8 ), 1 );
        FS::read( f, p_dataBuffer, sizeof( u32 ), width * height * frameCount / 8 );
        FS::close( f );

        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, width, height, p_palData,
                            reinterpret_cast<unsigned int*>( p_dataBuffer ), width * height / 2,
                            false, false, false, OBJPRIORITY_2, false );
    }

    void setOWSpriteFrame( u8 p_frame, bool p_flip, u8 p_oamIdx, u16 p_palData[ 16 ],
                           u32 p_dataBuffer[ 32 * 4 * 9 ] ) {
        u8 frame = p_frame;
        u8 wd = spriteInfoTop[ p_oamIdx ].m_width, hg = spriteInfoTop[ p_oamIdx ].m_height;

        loadSpriteB( p_oamIdx, OamTop->oamBuffer[ p_oamIdx ].gfxIndex,
                     OamTop->oamBuffer[ p_oamIdx ].x, OamTop->oamBuffer[ p_oamIdx ].y, wd, hg,
                     p_palData,
                     reinterpret_cast<unsigned int*>( p_dataBuffer ) + wd * hg * frame / 8,
                     wd * hg / 2, false, false, OamTop->oamBuffer[ p_oamIdx ].isHidden,
                     OamTop->oamBuffer[ p_oamIdx ].priority, false );

        OamTop->oamBuffer[ p_oamIdx ].hFlip = p_flip;
    }

    void setAnimatedSpriteFrame( u8 p_frame, bool p_hFlip, u8 p_oamIdx, u16 p_tileCnt ) {
        u8 width = spriteInfoTop[ p_oamIdx ].m_width, height = spriteInfoTop[ p_oamIdx ].m_height;

        OamTop->oamBuffer[ p_oamIdx ].hFlip    = p_hFlip;
        OamTop->oamBuffer[ p_oamIdx ].gfxIndex = p_tileCnt + p_frame * width * height / 64;
    }

    u16 loadIcon( const char* p_path, const char* p_name, const s16 p_posX, const s16 p_posY,
                  u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {
        if( FS::readData( p_path, p_name, (unsigned int) 128, TEMP, (unsigned short) 16,
                          TEMP_PAL ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL,
                               TEMP, 512, false, false, false,
                               p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        } else {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal,
                               NoItemTiles, NoItemTilesLen, false, false, false,
                               p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }
    }
    u16 loadIconB( const char* p_path, const char* p_name, const s16 p_posX, const s16 p_posY,
                   u8 p_oamIdx, u16 p_tileCnt, bool p_bottom ) {
        if( FS::readData( p_path, p_name, (unsigned int) 128, TEMP, (unsigned short) 16,
                          TEMP_PAL ) ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP, 512,
                                false, false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0,
                                p_bottom );
        } else {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal, NoItemTiles,
                                NoItemTilesLen, false, false, false,
                                p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }
    }

    u16 loadPKMNIcon( const u16 p_pkmnId, const u16 p_posX, const u16 p_posY, u8 p_oamIdx,
                      u8 p_palCnt, u16 p_tileCnt, bool p_bottom, u8 p_forme, bool p_shiny,
                      bool p_female ) {
        FILE* f;
        /*
        if( !existsPKMNSprite( p_pkmnId, true, p_female ) ) {
            // TODO: Handle existing flipx sprites
            p_flipx = false;
        }
        */
        if( !existsPKMNSprite( p_pkmnId, false, p_female ) ) { p_female = false; }

        if( p_forme ) {
            snprintf( BUFFER, 99, "/icon%03hu%s%s_%hhu.rsd", p_pkmnId, p_female ? "f" : "",
                      p_shiny ? "s" : "", p_forme );

            f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
            if( f ) {
                return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                           OBJPRIORITY_2, p_bottom );
            }

            if( p_shiny ) {
                snprintf( BUFFER, 99, "/icon%03hu%s_%hhu.rsd", p_pkmnId, p_female ? "f" : "",
                          p_forme );
                f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
                if( f ) {
                    return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                               OBJPRIORITY_2, p_bottom );
                }
            }

            if( p_female ) {
                snprintf( BUFFER, 99, "/icon%03hu%s_%hhu.rsd", p_pkmnId, p_shiny ? "s" : "",
                          p_forme );
                f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
                if( f ) {
                    return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                               OBJPRIORITY_2, p_bottom );
                }
            }
        }

        snprintf( BUFFER, 99, "/icon%03hu%s%s.rsd", p_pkmnId, p_female ? "f" : "",
                  p_shiny ? "s" : "" );
        f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
        if( f ) {
            return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                       OBJPRIORITY_2, p_bottom );
        }
        if( p_shiny ) {
            snprintf( BUFFER, 99, "/icon%03hu%s.rsd", p_pkmnId, p_female ? "f" : "" );
            f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
            if( f ) {
                return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                           OBJPRIORITY_2, p_bottom );
            }
        }

        if( p_female ) {
            snprintf( BUFFER, 99, "/icon%03hu%s.rsd", p_pkmnId, p_shiny ? "s" : "" );
            f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
            if( f ) {
                return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                           OBJPRIORITY_2, p_bottom );
            }
        }

        snprintf( BUFFER, 99, "/icon%03hu.rsd", p_pkmnId );
        f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
        if( f ) {
            return loadAnimatedSprite( f, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                                       OBJPRIORITY_2, p_bottom );
        }
        return loadPKMNIcon( 0, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom );
    }

    u16 loadPKMNIconB( const u16 p_pkmnId, const u16 p_posX, const u16 p_posY, u8 p_oamIdx,
                       u16 p_tileCnt, bool p_bottom, u8 p_forme, bool p_shiny, bool p_female,
                       bool p_outline, u16 p_outlineColor ) {
        FILE* f;
        /*
        if( !existsPKMNSprite( p_pkmnId, true, p_female ) ) {
            // TODO: Handle existing flipx sprites
            p_flipx = false;
        }
        */
        if( !existsPKMNSprite( p_pkmnId, false, p_female ) ) { p_female = false; }

        if( p_forme ) {
            snprintf( BUFFER, 99, "/icon%03hu%s%s_%hhu.rsd", p_pkmnId, p_female ? "f" : "",
                      p_shiny ? "s" : "", p_forme );

            f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
            if( f ) {
                return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt, OBJPRIORITY_2,
                                            p_bottom, p_outline, p_outlineColor );
            }

            if( p_shiny ) {
                snprintf( BUFFER, 99, "/icon%03hu%s_%hhu.rsd", p_pkmnId, p_female ? "f" : "",
                          p_forme );
                f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
                if( f ) {
                    return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                                OBJPRIORITY_2, p_bottom, p_outline,
                                                p_outlineColor );
                }
            }

            if( p_female ) {
                snprintf( BUFFER, 99, "/icon%03hu%s_%hhu.rsd", p_pkmnId, p_shiny ? "s" : "",
                          p_forme );
                f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
                if( f ) {
                    return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt,
                                                OBJPRIORITY_2, p_bottom, p_outline,
                                                p_outlineColor );
                }
            }
        }

        snprintf( BUFFER, 99, "/icon%03hu%s%s.rsd", p_pkmnId, p_female ? "f" : "",
                  p_shiny ? "s" : "" );
        f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
        if( f ) {
            return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt, OBJPRIORITY_2,
                                        p_bottom, p_outline, p_outlineColor );
        }
        if( p_shiny ) {
            snprintf( BUFFER, 99, "/icon%03hu%s.rsd", p_pkmnId, p_female ? "f" : "" );
            f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
            if( f ) {
                return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt, OBJPRIORITY_2,
                                            p_bottom, p_outline, p_outlineColor );
            }
        }

        if( p_female ) {
            snprintf( BUFFER, 99, "/icon%03hu%s.rsd", p_pkmnId, p_shiny ? "s" : "" );
            f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
            if( f ) {
                return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt, OBJPRIORITY_2,
                                            p_bottom, p_outline, p_outlineColor );
            }
        }

        snprintf( BUFFER, 99, "/icon%03hu.rsd", p_pkmnId );
        f = FS::openSplit( PKMN_PATH, p_pkmnId, BUFFER );
        if( f ) {
            return loadAnimatedSpriteB( f, p_posX, p_posY, p_oamIdx, p_tileCnt, OBJPRIORITY_2,
                                        p_bottom, p_outline, p_outlineColor );
        }
        return loadPKMNIconB( 0, p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom, p_outline,
                              p_outlineColor );
    }

    u16 loadEggIcon( const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                     bool p_bottom, bool p_manaphy ) {
        return loadPKMNIcon( 1 - 1, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom,
                             1 + p_manaphy, false, false );
    }

    u16 loadEggIconB( const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u16 p_tileCnt, bool p_bottom,
                      bool p_manaphy, bool p_outline, u16 p_outlineColor ) {
        return loadPKMNIconB( 1 - 1, p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom, 1 + p_manaphy,
                              false, false, p_outline, p_outlineColor );
    }

    u16 loadItemIcon( u16 p_itemId, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                      u16 p_tileCnt, bool p_bottom ) {
        FILE* f = FS::openSplit( ITEM_PATH, p_itemId, ".raw" );
        if( !f ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal,
                               NoItemTiles, NoItemTilesLen, false, false, false,
                               p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }

        FS::read( f, TEMP, sizeof( u32 ), 128 );
        FS::read( f, TEMP_PAL, sizeof( u16 ), 16 );

        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP,
                           512, false, false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0,
                           p_bottom );
    }

    u16 loadItemIconB( u16 p_itemId, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       bool p_bottom ) {
        FILE* f = FS::openSplit( ITEM_PATH, p_itemId, ".raw" );
        if( !f ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal, NoItemTiles,
                                NoItemTilesLen, false, false, false,
                                p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }

        FS::read( f, TEMP, sizeof( u32 ), 128 );
        FS::read( f, TEMP_PAL, sizeof( u16 ), 16 );

        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP, 512, false,
                            false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
    }

    u16 loadTMIcon( type p_type, bool p_hm, const u16 p_posX, const u16 p_posY, u8 p_oamIdx,
                    u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {
        std::string itemname
            = ( p_hm ? "VM" : "TM" )
              + ( std::vector<std::string>{ "Normal", "Kampf", "Flug", "Gift", "Boden", "Gestein",
                                            "Pflanze", "Geist", "Stahl", "Unbekannt", "Wasser",
                                            "Feuer", "Pflanze", "Elektro", "Psycho", "Eis",
                                            "Drache", "Unlicht", "Fee" }[ p_type ] );

        return loadIcon( TM_PATH, itemname.c_str( ), p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt,
                         p_bottom );
    }

    u16 loadTMIconB( type p_type, bool p_hm, const u16 p_posX, const u16 p_posY, u8 p_oamIdx,
                     u16 p_tileCnt, bool p_bottom ) {
        std::string itemname
            = ( p_hm ? "VM" : "TM" )
              + ( std::vector<std::string>{ "Normal", "Kampf", "Flug", "Gift", "Boden", "Gestein",
                                            "Pflanze", "Geist", "Stahl", "Unbekannt", "Wasser",
                                            "Feuer", "Pflanze", "Elektro", "Psycho", "Eis",
                                            "Drache", "Unlicht", "Fee" }[ p_type ] );

        return loadIconB( TM_PATH, itemname.c_str( ), p_posX, p_posY, p_oamIdx, p_tileCnt,
                          p_bottom );
    }

    u16 loadTypeIcon( type p_type, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                      u16 p_tileCnt, bool p_bottom, const SAVE::language p_language ) {
        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 16,
                           TypePals[ p_type ][ p_language ], TypeTiles[ p_type ][ p_language ], 256,
                           false, false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadTypeIconB( type p_type, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       bool p_bottom, const SAVE::language p_language ) {
        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 16,
                            TypePals[ p_type ][ p_language ], TypeTiles[ p_type ][ p_language ],
                            256, false, false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadLocationBackB( u8 p_idx, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                           bool p_bottom ) {
        if( FS::readData<unsigned short, unsigned int>( "nitro:/PICS/SPRITES/LOC/",
                                                        ( std::to_string( p_idx ) ).c_str( ), 16,
                                                        TEMP_PAL, 128 * 32 / 8, TEMP ) ) {
            loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 64, 32, TEMP_PAL, TEMP, 64 * 32 / 2,
                         false, false, false, OBJPRIORITY_0, p_bottom );
            return loadSpriteB( ++p_oamIdx, p_tileCnt + 64, p_posX + 64, p_posY, 64, 32, TEMP_PAL,
                                TEMP + 64 * 32 / 8, 64 * 32 / 2, false, false, false, OBJPRIORITY_0,
                                p_bottom );
        } else {
            return loadLocationBackB( 0, p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom );
        }
    }
    u16 loadPlatform( u8 p_platform, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                      u16 p_tileCnt, bool p_bottom ) {

        if( FS::readData<unsigned short, unsigned int>(
                "nitro:/PICS/SPRITES/PLAT/", ( "plat" + std::to_string( p_platform ) ).c_str( ), 16,
                TEMP_PAL, 128 * 64 / 8, TEMP ) ) {
            loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 64, 64, TEMP_PAL, TEMP,
                        128 * 64 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        p_platform > 40 ? OBJMODE_BLENDED : OBJMODE_NORMAL );
            return loadSprite( ++p_oamIdx, p_palCnt, p_tileCnt + 64, p_posX + 64, p_posY, 64, 64, 0,
                               0, 64 * 64 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                               p_platform > 40 ? OBJMODE_BLENDED : OBJMODE_NORMAL );
        } else {
            return loadPlatform( 10, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom );
        }
    }

    u16 loadRibbonIcon( u8 p_ribbonIdx, const u16 p_posX, const u16 p_posY, u8 p_oamIdx,
                        u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {

        if( FS::readData( "nitro:/PICS/SPRITES/RIBBON/",
                          ( "r" + std::to_string( p_ribbonIdx ) ).c_str( ),
                          (unsigned int) 32 * 32 / 8, TEMP, (unsigned short) 16, TEMP_PAL ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL,
                               TEMP, 32 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
        }
        return p_tileCnt + ( 32 * 16 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 loadShapeIcon( u8 p_shapeIdx, const u16 p_posX, const u16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                       u16 p_tileCnt, bool p_bottom ) {

        if( FS::readData( "nitro:/PICS/SPRITES/SHAPES/", std::to_string( p_shapeIdx ).c_str( ),
                          (unsigned int) 32 * 32 / 8, TEMP, (unsigned short) 16, TEMP_PAL ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL,
                               TEMP, 32 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
        }
        return p_tileCnt + ( 32 * 16 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 loadDamageCategoryIcon( MOVE::moveHitTypes p_type, const u16 p_posX, const u16 p_posY,
                                u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {
        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 16,
                           HitTypePals[ p_type - 1 ], HitTypeTiles[ p_type - 1 ], 256, false, false,
                           false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadDamageCategoryIconB( MOVE::moveHitTypes p_type, const u16 p_posX, const u16 p_posY,
                                 u8 p_oamIdx, u16 p_tileCnt, bool p_bottom ) {
        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 16, HitTypePals[ p_type - 1 ],
                            HitTypeTiles[ p_type - 1 ], 256, false, false, false, OBJPRIORITY_0,
                            p_bottom );
    }

} // namespace IO
