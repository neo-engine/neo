/*
Pokémon neo
------------------------------

file        : sprite.cpp
author      : Philip Wellnitz
description : Some sprite code.

Copyright (C) 2012 - 2022
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

#include "fs/fs.h"
#include "gen/pokemonNames.h"
#include "io/sprite.h"
#include "io/uio.h"

#include "NoItem.h"

unsigned int   TEMP[ 256 * 256 / 4 ] = { 0 };
unsigned short TEMP_PAL[ 256 ]       = { 0 };

namespace IO {
    const char* OW_PATH                = "nitro:/PICS/SPRITES/OW/";
    const char* DOOR_PATH              = "nitro:/PICS/SPRITES/DOOR/";
    const char* OWP_PATH               = "nitro:/PICS/SPRITES/NPCP/";
    const char* TRAINER_PATH           = "nitro:/PICS/SPRITES/NPC/";
    const char* BERRY_PATH             = "nitro:/PICS/SPRITES/BERRIES/";
    const char* ICON_PATH              = "nitro:/PICS/SPRITES/ICONS/";
    const char* TYPE_ICON_PATH         = "nitro:/PICS/SPRITES/ICONS/TP/type";
    const char* CONTEST_TYPE_ICON_PATH = "nitro:/PICS/SPRITES/ICONS/TP/ctype";

    const char* ITEM_PATH      = "nitro:/PICS/SPRITES/item.icon.rawb";
    FILE*       ITEM_ICON_FILE = nullptr;

    const char* TM_PATH      = "nitro:/PICS/SPRITES/tmhm.icon.rawb";
    FILE*       TM_ICON_FILE = nullptr;

    const char* PKMN_ICON_PATH = "nitro:/PICS/SPRITES/icon";
    const char* PKMN_PATH      = "nitro:/PICS/SPRITES/frnt";
    const char* PKMN_BACK_PATH = "nitro:/PICS/SPRITES/back";
    // (female, shiny); (0,0), (0,1), (1,0), (1,1)
    FILE* PKMN_SPRITE_ICON_FILES[ 4 ] = { nullptr, nullptr, nullptr, nullptr };
    FILE* PKMN_SPRITE_FRNT_FILES[ 4 ] = { nullptr, nullptr, nullptr, nullptr };
    FILE* PKMN_SPRITE_BACK_FILES[ 4 ] = { nullptr, nullptr, nullptr, nullptr };

    const u8  SPRITE_DMA_CHANNEL      = 2;
    const u16 BYTES_PER_16_COLOR_TILE = 32;
    const u16 COLORS_PER_PALETTE      = 16;
    const u16 BOUNDARY_VALUE          = 32;
    const u16 OFFSET_MULTIPLIER       = ( BOUNDARY_VALUE / sizeof( SPRITE_GFX[ 0 ] ) );
    const u16 OFFSET_MULTIPLIER_SUB   = ( BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] ) );

    // Lower-y of bounding box of sprite
    // (identify -format "%[bounding-box]" $i.png | string match -r '\d+$')
    const std::vector<u16> SPECIES_HEIGHTS{
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
        84, 66, 80, 72, 88, 66, 74, 87, 73, 78, 87, 88, 78, 91, 95, 76, 86, 65, 75, 74, 68, 62, 91,
        86, 89, 74, 73, 74, 83, 84, 88, 89, 80, 85, 78, 89, 87, 95, 92, 96, 75, 88, 82, 85, 84, 87,
        87, 90,
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

        if( p_bottom ) {
            // This is bad style, but fast.
            memset( SPRITE_GFX_SUB, 0, 1024 );
        } else {
            // This is bad style, but fast.
            memset( SPRITE_GFX, 0, 1024 );
        }
        updateOAM( p_bottom );
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

    u16 setSpriteData( const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileCnt, const s16 p_posX,
                       const s16 p_posY, const u8 p_width, const u8 p_height,
                       const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                       ObjPriority p_priority, bool p_bottom, ObjBlendMode p_blendMode,
                       ObjColMode p_colorMode ) {
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
        spriteEntry->colorMode     = p_colorMode;

        spriteEntry->shape = ( ( p_width == p_height )
                                   ? OBJSHAPE_SQUARE
                                   : ( ( p_width > p_height ) ? OBJSHAPE_WIDE : OBJSHAPE_TALL ) );

        u8 maxSize = std::max( p_width, p_height );
        spriteEntry->size
            = ( ( maxSize == 64 )
                    ? OBJSIZE_64
                    : ( ( maxSize == 32 ) ? OBJSIZE_32
                                          : ( ( maxSize == 16 ) ? OBJSIZE_16 : OBJSIZE_8 ) ) );

        return p_tileCnt + ( p_spriteDataLen / BYTES_PER_16_COLOR_TILE );
    }

    u16 loadSprite( const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileCnt, const s16 p_posX,
                    const s16 p_posY, const u8 p_width, const u8 p_height,
                    const unsigned short* p_spritePal, const unsigned int* p_spriteData,
                    const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                    ObjPriority p_priority, bool p_bottom, ObjBlendMode p_blendMode ) {
        auto res = setSpriteData( p_oamIdx, p_palIdx, p_tileCnt, p_posX, p_posY, p_width, p_height,
                                  p_spriteDataLen, p_flipX, p_flipY, p_hidden, p_priority, p_bottom,
                                  p_blendMode, OBJCOLOR_16 );
        copySpriteData( p_spriteData, p_tileCnt, p_spriteDataLen, p_bottom );
        copySpritePal( p_spritePal, p_palIdx, p_bottom );
        return res;
    }

    u16 loadSpriteB( const u8 p_oamIdx, const u16 p_tileCnt, const s16 p_posX, const s16 p_posY,
                     const u8 p_width, const u8 p_height, const unsigned short* p_spriteData,
                     const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                     ObjPriority p_priority, bool p_bottom, u8 p_transparency ) {

        auto res = setSpriteData( p_oamIdx, p_transparency, p_tileCnt, p_posX, p_posY, p_width,
                                  p_height, p_spriteDataLen, p_flipX, p_flipY, p_hidden, p_priority,
                                  p_bottom, (ObjBlendMode) SpriteColorFormat_Bmp, (ObjColMode) 0 );
        auto gfx = p_bottom ? &SPRITE_GFX_SUB[ (u32) p_tileCnt * 64 ]
                            : &SPRITE_GFX[ (u32) p_tileCnt * 64 ];
        if( p_spriteData ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, p_spriteData, gfx, p_width * p_height * 2 );
        }
        return res;
    }

    u16 loadSprite( const char* p_name, const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileCnt,
                    const s16 p_posX, const s16 p_posY, const u8 p_width, const u8 p_height,
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

    u16 BITMAP_SPRITE[ 64 * 64 * 2 ] = { 0 };
    u16 loadSpriteB( const u8 p_oamIdx, const u16 p_tileCnt, const s16 p_posX, const s16 p_posY,
                     const u8 p_width, const u8 p_height, const unsigned short* p_spritePal,
                     const unsigned int* p_spriteData, const u32 p_spriteDataLen, bool p_flipX,
                     bool p_flipY, bool p_hidden, ObjPriority p_priority, bool p_bottom,
                     bool p_outline, u16 p_outlineColor, bool p_tiled, u8 p_transparency ) {
        memset( BITMAP_SPRITE, 0, sizeof( BITMAP_SPRITE ) );
        const u8* spriteData = reinterpret_cast<const u8*>( p_spriteData );
        if( p_spritePal && p_spriteData && p_tiled ) {
            u32 i = 0;
            for( u8 tiley = 0; tiley < p_height / 8; ++tiley ) {
                for( u8 tilex = 0; tilex < p_width / 8; ++tilex ) {
                    s32 shift = tilex * ( -28 ); // size per tile
                    for( u8 y = 0; y < 8; ++y ) {
                        for( u8 x = 0; x < 8; x += 2, ++i ) {
                            u8 cur = spriteData[ i ];
                            u8 up = ( cur >> 4 ) & 0xf, down = cur & 0xf;
                            BITMAP_SPRITE[ 2 * ( i + shift ) + 1 ]
                                = up ? ( ( 1 << 15 ) | p_spritePal[ up ] ) : 0;
                            BITMAP_SPRITE[ 2 * ( i + shift ) ]
                                = down ? ( ( 1 << 15 ) | p_spritePal[ down ] ) : 0;
                        }
                        shift += 4 * ( p_width / 8 - 1 );
                    }
                }
            }
        } else if( p_spritePal && p_spriteData ) {
            u32 i = 0;
            for( u8 y = 0; y < p_height; ++y ) {
                for( u8 x = 0; x < p_width; x += 2, ++i ) {
                    u8 cur = spriteData[ i ];
                    u8 up = cur >> 4, down = cur & 0xf;
                    BITMAP_SPRITE[ 2 * ( i ) + 1 ] = up ? ( ( 1 << 15 ) | p_spritePal[ up ] ) : 0;
                    BITMAP_SPRITE[ 2 * ( i ) ] = down ? ( ( 1 << 15 ) | p_spritePal[ down ] ) : 0;
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
                            p_flipX, p_flipY, p_hidden, p_priority, p_bottom, p_transparency );
    }

    u16 loadSpriteB( const char* p_name, const u8 p_oamIdx, const u16 p_tileCnt, const s16 p_posX,
                     const s16 p_posY, const u8 p_width, const u8 p_height, bool p_flipX,
                     bool p_flipY, bool p_hidden, ObjPriority p_priority, bool p_bottom,
                     bool p_outline, u16 p_outlineColor, bool p_tiled, u8 p_transparency ) {
        if( FS::readData( ICON_PATH, p_name, (unsigned int) p_width * p_height / 8, TEMP,
                          (unsigned short) 16, TEMP_PAL ) ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, p_width, p_height, TEMP_PAL,
                                TEMP, p_width * p_height / 2, p_flipX, p_flipY, p_hidden,
                                p_priority, p_bottom, p_outline, p_outlineColor, p_tiled,
                                p_transparency );
        }
        return p_tileCnt + ( p_width * p_height / 2 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 pkmnSpriteHeight( const pkmnSpriteInfo& p_pkmn ) {
        if( p_pkmn.m_pkmnIdx > MAX_PKMN ) { return 96 - SPECIES_HEIGHTS[ 0 ]; }
        return 96 - SPECIES_HEIGHTS[ p_pkmn.m_pkmnIdx ];
    }

    char           BUFFER[ 150 ];
    constexpr bool existsPKMNSprite( const pkmnSpriteInfo& p_pkmn ) {
        if( p_pkmn.m_flipX ) {
            switch( p_pkmn.m_pkmnIdx ) {
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
        return true;
    }

    FILE* checkOrOpenPKMNFile( FILE* p_files[ 4 ], const char* p_path, bool p_female,
                               bool p_shiny ) {
        FILE* f = p_files[ 2 * p_female + p_shiny ];

        if( f == nullptr ) {
            snprintf( BUFFER, 149, "%s%s%s.pkmn.sprb", p_path, p_female ? "f" : "",
                      p_shiny ? "s" : "" );
            f = p_files[ 2 * p_female + p_shiny ] = fopen( BUFFER, "rb" );
        }
        return f;
    }

    bool seekSpriteData( FILE* p_file, u16 p_idx, u16 p_dataSize = 16 * 32 / 8 ) {
        if( fseek( p_file, p_idx * ( 16 * sizeof( u16 ) + p_dataSize * sizeof( u32 ) ),
                   SEEK_SET ) ) {
            return false;
        }
        return true;
    }

    bool loadPKMNSpriteData( FILE* p_files[ 4 ], const char* p_path, const pkmnSpriteInfo& p_pkmn,
                             bool p_blackOverlay, u16 p_dataSize = 96 * 96 / 8 ) {
        FILE* f = nullptr;
        if( !p_pkmn.m_forme ) {
            if( !( f = checkOrOpenPKMNFile( p_files, p_path, p_pkmn.m_female, p_pkmn.m_shiny ) ) ) {
                return false;
            }
            if( !seekSpriteData( f, p_pkmn.m_pkmnIdx, p_dataSize ) ) {
                if( f ) { fclose( f ); }
                return false;
            }
        } else {
            snprintf( BUFFER, 149, "%s/%d/%d_%hhu%s%s.raw", p_path,
                      p_pkmn.m_pkmnIdx / ITEMS_PER_DIR, p_pkmn.m_pkmnIdx, p_pkmn.m_forme,
                      p_pkmn.m_female ? "f" : "", p_pkmn.m_shiny ? "s" : "" );
            f = fopen( BUFFER, "rb" );
            if( f == nullptr ) { return false; }
        }

        if( !f ) { return false; }
        if( !fread( TEMP_PAL, 16, sizeof( u16 ), f ) ) {
            fclose( f );
            return false;
        }
        if( !fread( TEMP, p_dataSize, sizeof( u32 ), f ) ) {
            fclose( f );
            return false;
        }

        // small color variation for pkmn of the same species
        for( u8 i = 1; i < 16; ++i ) {
            auto pid = p_pkmn.m_pid >> ( i / 2 );
            auto r = !!( pid & 0b1 ), g = !!( pid & 0b10 ), b = !!( pid & 0b1000 );
            TEMP_PAL[ i ] ^= ( g << 10 ) | ( b << 5 ) | r;
        }

        if( p_pkmn.m_pkmnIdx == PKMN_SPINDA && !strcmp( p_path, PKMN_PATH ) ) [[unlikely]] {
            // create spinda spots; ensure that spots are in the top-left 64x64 corner
            constexpr u8  SPOT_COUNT         = 4;
            constexpr u8  SPOT_SIZE          = 8;
            constexpr u8  SPOT_BG_COLOR      = 2;
            constexpr u8  SPOT_COLOR         = 7;
            constexpr u8  SPOT_BG_COLOR_SHDW = 5;
            constexpr u8  SPOT_COLOR_SHDW    = 8;
            constexpr u8  SX[ SPOT_COUNT ]   = { 35 - 8 - 6, 55 - 8 - 6, 37 - 8 - 6, 49 - 8 - 6 };
            constexpr u8  SY[ SPOT_COUNT ]   = { 29 - 8 - 6, 29 - 8 - 6, 45 - 8 - 6, 46 - 8 - 6 };
            constexpr u16 DOT_PATTERN[ SPOT_SIZE ]
                = { 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c };

            for( u8 i = 0; i < SPOT_COUNT; ++i ) {
                u8 dx = ( p_pkmn.m_pid >> ( 2 * i ) ) & 0xF;
                u8 dy = ( p_pkmn.m_pid >> ( 2 * i + 1 ) ) & 0xF;

                for( u8 x = 0; x < SPOT_SIZE; ++x ) {
                    for( u8 y = 0; y < SPOT_SIZE; ++y ) {
                        if( !( DOT_PATTERN[ y ] & ( 1 << x ) ) ) { continue; }
                        // check if pixel has spinda bg color; if so, recolor to spot
                        // color

                        u8 nx    = SX[ i ] + dx + x;
                        u8 tilex = nx >> 3, nxm8 = nx & 7;
                        u8 ny    = SY[ i ] + dy + y;
                        u8 tiley = ny >> 3, nym8 = ny & 7;

                        u16 pos = 64 * tiley + 8 * tilex + nym8;

                        u32 px = TEMP[ pos ];
                        u8  cl = ( px >> ( nxm8 * 4 ) ) & 0xF;
                        if( cl == SPOT_BG_COLOR ) { cl = SPOT_COLOR; }
                        if( cl == SPOT_BG_COLOR_SHDW ) { cl = SPOT_COLOR_SHDW; }
                        TEMP[ pos ] = ( px & ~( 0xF << ( nxm8 * 4 ) ) ) | ( cl << ( nxm8 * 4 ) );
                    }
                }
            }
        }

        if( p_blackOverlay ) { std::memset( TEMP_PAL, 0, sizeof( TEMP_PAL ) ); }

        if( p_pkmn.m_forme && f ) { fclose( f ); }

        return true;
    }

    u16 loadPKMNSprite( const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                        bool p_bottom, bool p_flipx ) {
        loadSprite( p_oamIdx++, p_palCnt, p_tileCnt, p_flipx ? 32 + p_posX : p_posX, p_posY, 64, 64,
                    TEMP_PAL, TEMP, 96 * 96 / 2, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        loadSprite( p_oamIdx++, p_palCnt, p_tileCnt + 64, p_flipx ? p_posX : 64 + p_posX, p_posY,
                    32, 64, 0, 0, 0, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        loadSprite( p_oamIdx++, p_palCnt, p_tileCnt + 96, p_flipx ? 32 + p_posX : p_posX,
                    p_posY + 64, 64, 32, 0, 0, 0, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        loadSprite( p_oamIdx, p_palCnt, p_tileCnt + 128, p_flipx ? p_posX : 64 + p_posX,
                    p_posY + 64, 32, 32, 0, 0, 0, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        updateOAM( p_bottom );
        return p_tileCnt + 144;
    }

    u16 loadPKMNSpriteB( const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                         bool p_bottom, bool p_flipx ) {
        loadSpriteB( p_oamIdx++, p_tileCnt, p_flipx ? 32 + p_posX : p_posX, p_posY, 64, 64,
                     TEMP_PAL, TEMP, 64 * 64 / 2, false, p_flipx, false, OBJPRIORITY_1, p_bottom );
        loadSpriteB( p_oamIdx++, p_tileCnt + 64, p_flipx ? p_posX : 64 + p_posX, p_posY, 32, 64,
                     TEMP_PAL, TEMP + 64 * 64 / 8, 64 * 32 / 2, false, p_flipx, false,
                     OBJPRIORITY_1, p_bottom );
        loadSpriteB( p_oamIdx++, p_tileCnt + 96, p_flipx ? 32 + p_posX : p_posX, p_posY + 64, 64,
                     32, TEMP_PAL, TEMP + 64 * 64 / 8 + 32 * 64 / 8, 32 * 64 / 2, false, p_flipx,
                     false, OBJPRIORITY_1, p_bottom );
        loadSpriteB( p_oamIdx, p_tileCnt + 128, p_flipx ? p_posX : 64 + p_posX, p_posY + 64, 32, 32,
                     TEMP_PAL, TEMP + 64 * 64 / 8 + 64 * 64 / 8, 32 * 32 / 8, false, p_flipx, false,
                     OBJPRIORITY_1, p_bottom );

        updateOAM( p_bottom );
        return p_tileCnt + 144;
    }

    void updateFlipX( pkmnSpriteInfo& p_pkmn, bool& p_flipX ) {
        if( !existsPKMNSprite( p_pkmn ) ) {
            // if there is no separate flipped image, flip manually
            p_flipX = p_pkmn.m_flipX;
            p_pkmn.m_flipX ^= 1;
        }
    }

    u16 loadPKMNSprite( FILE* p_files[ 4 ], const char* p_path, const pkmnSpriteInfo& p_pkmn,
                        const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                        bool p_bottom, bool p_blackOverlay ) {
        bool           fx = false;
        pkmnSpriteInfo pi = p_pkmn;
        updateFlipX( pi, fx );
        if( loadPKMNSpriteData( p_files, p_path, pi, p_blackOverlay ) ) {
            return loadPKMNSprite( p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom, fx );
        }
        return 0;
    }

    u16 loadPKMNSpriteB( FILE* p_files[ 4 ], const char* p_path, const pkmnSpriteInfo& p_pkmn,
                         const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                         bool p_bottom, bool p_blackOverlay ) {
        bool           fx = false;
        pkmnSpriteInfo pi = p_pkmn;
        updateFlipX( pi, fx );
        if( loadPKMNSpriteData( p_files, p_path, pi, p_blackOverlay ) ) {
            return loadPKMNSpriteB( p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom, fx );
        }
        return 0;
    }

    u16 loadPKMNSprite( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                        u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom,
                        bool p_blackOverlay ) {
        u16 res = 0;
        if( ( res = loadPKMNSprite( PKMN_SPRITE_FRNT_FILES, PKMN_PATH, p_pkmn, p_posX, p_posY,
                                    p_oamIdx, p_palCnt, p_tileCnt, p_bottom, p_blackOverlay ) ) ) {
            return res;
        }

        pkmnSpriteInfo nf = p_pkmn;
        nf.m_forme        = 0;
        return loadPKMNSprite( PKMN_SPRITE_FRNT_FILES, PKMN_PATH, nf, p_posX, p_posY, p_oamIdx,
                               p_palCnt, p_tileCnt, p_bottom, p_blackOverlay );
    }

    u16 loadPKMNSpriteB( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                         u8 p_oamIdx, u16 p_tileCnt, bool p_bottom, bool p_blackOverlay ) {
        u16 res = 0;
        if( ( res = loadPKMNSpriteB( PKMN_SPRITE_FRNT_FILES, PKMN_PATH, p_pkmn, p_posX, p_posY,
                                     p_oamIdx, p_tileCnt, p_bottom, p_blackOverlay ) ) ) {
            return res;
        }

        pkmnSpriteInfo nf = p_pkmn;
        nf.m_forme        = 0;
        return loadPKMNSpriteB( PKMN_SPRITE_FRNT_FILES, PKMN_PATH, nf, p_posX, p_posY, p_oamIdx,
                                p_tileCnt, p_bottom, p_blackOverlay );
    }

    u16 loadPKMNSpriteBack( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                            u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom,
                            bool p_blackOverlay ) {
        u16 res = 0;
        if( ( res = loadPKMNSprite( PKMN_SPRITE_BACK_FILES, PKMN_BACK_PATH, p_pkmn, p_posX, p_posY,
                                    p_oamIdx, p_palCnt, p_tileCnt, p_bottom, p_blackOverlay ) ) ) {
            return res;
        }

        pkmnSpriteInfo nf = p_pkmn;
        nf.m_forme        = 0;
        return loadPKMNSprite( PKMN_SPRITE_BACK_FILES, PKMN_BACK_PATH, nf, p_posX, p_posY, p_oamIdx,
                               p_palCnt, p_tileCnt, p_bottom, p_blackOverlay );
    }

    u16 loadEggSprite( const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                       bool p_bottom, bool p_manaphy ) {
        pkmnSpriteInfo pinfo = { 0, u8( 1 + p_manaphy ), false, false, false, DEFAULT_SPRITE_PID };
        return loadPKMNSprite( pinfo, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom );
    }

    u16 loadPKMNIcon( FILE* p_files[ 4 ], const char* p_path, const pkmnSpriteInfo& p_pkmn,
                      const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                      bool p_bottom, bool p_blackOverlay ) {
        bool           fx = false;
        pkmnSpriteInfo pi = p_pkmn;
        updateFlipX( pi, fx );
        if( loadPKMNSpriteData( p_files, p_path, pi, p_blackOverlay, 32 * 32 / 8 ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL,
                               TEMP, 512, fx, false, false,
                               p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }
        return 0;
    }

    u16 loadPKMNIconB( FILE* p_files[ 4 ], const char* p_path, const pkmnSpriteInfo& p_pkmn,
                       const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       bool p_bottom, bool p_outline, u16 p_outlineColor, bool p_blackOverlay ) {
        bool           fx = false;
        pkmnSpriteInfo pi = p_pkmn;
        updateFlipX( pi, fx );
        if( loadPKMNSpriteData( p_files, p_path, pi, p_blackOverlay, 32 * 32 / 8 ) ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP, 512,
                                fx, false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0,
                                p_bottom, p_outline, p_outlineColor );
        }
        return 0;
    }

    u16 loadPKMNIcon( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                      u8 p_palCnt, u16 p_tileCnt, bool p_bottom, bool p_blackOverlay ) {
        u16 res = 0;
        if( ( res = loadPKMNIcon( PKMN_SPRITE_ICON_FILES, PKMN_ICON_PATH, p_pkmn, p_posX, p_posY,
                                  p_oamIdx, p_palCnt, p_tileCnt, p_bottom, p_blackOverlay ) ) ) {
            return res;
        }

        pkmnSpriteInfo nf = p_pkmn;
        nf.m_forme        = 0;
        return loadPKMNIcon( PKMN_SPRITE_ICON_FILES, PKMN_ICON_PATH, nf, p_posX, p_posY, p_oamIdx,
                             p_palCnt, p_tileCnt, p_bottom, p_blackOverlay );
    }

    u16 loadPKMNIconB( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                       u8 p_oamIdx, u16 p_tileCnt, bool p_bottom, bool p_outline,
                       u16 p_outlineColor, bool p_blackOverlay ) {
        u16 res = 0;
        if( ( res = loadPKMNIconB( PKMN_SPRITE_ICON_FILES, PKMN_ICON_PATH, p_pkmn, p_posX, p_posY,
                                   p_oamIdx, p_tileCnt, p_bottom, p_outline, p_outlineColor,
                                   p_blackOverlay ) ) ) {
            return res;
        }

        pkmnSpriteInfo nf = p_pkmn;
        nf.m_forme        = 0;
        return loadPKMNIconB( PKMN_SPRITE_ICON_FILES, PKMN_ICON_PATH, nf, p_posX, p_posY, p_oamIdx,
                              p_tileCnt, p_bottom, p_outline, p_outlineColor, p_blackOverlay );
    }

    u16 loadEggIcon( const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt,
                     bool p_bottom, bool p_manaphy ) {
        pkmnSpriteInfo pinfo = { 0, u8( 1 + p_manaphy ), false, false, false, DEFAULT_SPRITE_PID };
        return loadPKMNIcon( pinfo, p_posX, p_posY, p_oamIdx, p_palCnt, p_tileCnt, p_bottom );
    }

    u16 loadEggIconB( const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u16 p_tileCnt, bool p_bottom,
                      bool p_manaphy, bool p_outline, u16 p_outlineColor ) {
        pkmnSpriteInfo pinfo = { 0, u8( 1 + p_manaphy ), false, false, false, DEFAULT_SPRITE_PID };
        return loadPKMNIconB( pinfo, p_posX, p_posY, p_oamIdx, p_tileCnt, p_bottom, p_outline,
                              p_outlineColor );
    }

    u16 loadTrainerSprite( u8 p_trainerId, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
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
                             u16 p_outlineColor, bool p_blackOverlay ) {
        FS::read( p_file, TEMP_PAL, sizeof( u16 ), 16 );
        u8 frameCount, width, height;
        FS::read( p_file, &frameCount, sizeof( u8 ), 1 );
        FS::read( p_file, &width, sizeof( u8 ), 1 );
        FS::read( p_file, &height, sizeof( u8 ), 1 );
        FS::read( p_file, TEMP, sizeof( u32 ), width * height * frameCount / 8 );
        FS::close( p_file );

        if( p_blackOverlay ) { std::memset( TEMP_PAL, 0, sizeof( TEMP_PAL ) ); }

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

    u16 loadDoorSpriteB( const u16 p_doorNum, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                         u16 p_tileCnt, u16 p_palData[ 16 ], u32 p_dataBuffer[ 32 * 4 * 9 ] ) {
        FILE* f = FS::openSplit( DOOR_PATH, p_doorNum, ".door", 255 );
        FS::read( f, p_dataBuffer, sizeof( u32 ), 32 * 16 * 3 / 8 );
        FS::close( f );

        return loadSpriteB( p_oamIndex, p_tileCnt, p_posX, p_posY, 16, 32, p_palData,
                            reinterpret_cast<unsigned int*>( p_dataBuffer ), 32 * 16 / 2, false,
                            false, false, OBJPRIORITY_2, false );
    }

    void setOWSpriteFrame( u8 p_frame, bool p_flip, u8 p_oamIdx, u16 p_palData[ 16 ],
                           u32 p_dataBuffer[ 32 * 4 * 9 ], bool p_vflip ) {
        u8 frame = p_frame;
        u8 wd = spriteInfoTop[ p_oamIdx ].m_width, hg = spriteInfoTop[ p_oamIdx ].m_height;

        loadSpriteB( p_oamIdx, OamTop->oamBuffer[ p_oamIdx ].gfxIndex,
                     OamTop->oamBuffer[ p_oamIdx ].x, OamTop->oamBuffer[ p_oamIdx ].y, wd, hg,
                     p_palData,
                     reinterpret_cast<unsigned int*>( p_dataBuffer ) + wd * hg * frame / 8,
                     wd * hg / 2, false, false, OamTop->oamBuffer[ p_oamIdx ].isHidden,
                     OamTop->oamBuffer[ p_oamIdx ].priority, false );

        OamTop->oamBuffer[ p_oamIdx ].hFlip = p_flip;
        OamTop->oamBuffer[ p_oamIdx ].vFlip = p_vflip;
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

    u16 loadItemIcon( u16 p_itemId, const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                      u16 p_tileCnt, bool p_bottom ) {
        if( !FS::checkOrOpen( ITEM_ICON_FILE, ITEM_PATH )
            || !seekSpriteData( ITEM_ICON_FILE, p_itemId, 32 * 32 / 8 )
            || !fread( TEMP_PAL, 16, sizeof( u16 ), ITEM_ICON_FILE )
            || !fread( TEMP, 32 * 32 / 8, sizeof( u32 ), ITEM_ICON_FILE ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal,
                               NoItemTiles, NoItemTilesLen, false, false, false,
                               p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }

        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP,
                           512, false, false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0,
                           p_bottom );
    }

    u16 loadItemIconB( u16 p_itemId, const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       bool p_bottom ) {
        if( !FS::checkOrOpen( ITEM_ICON_FILE, ITEM_PATH )
            || !seekSpriteData( ITEM_ICON_FILE, p_itemId, 32 * 32 / 8 )
            || !fread( TEMP_PAL, 16, sizeof( u16 ), ITEM_ICON_FILE )
            || !fread( TEMP, 32 * 32 / 8, sizeof( u32 ), ITEM_ICON_FILE ) ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal, NoItemTiles,
                                NoItemTilesLen, false, false, false,
                                p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }
        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP, 512, false,
                            false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
    }

    u16 loadTMIcon( BATTLE::type p_type, u8 p_tmtype, const s16 p_posX, const s16 p_posY,
                    u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {
        if( !FS::checkOrOpen( TM_ICON_FILE, TM_PATH )
            || !seekSpriteData( TM_ICON_FILE, 3 * p_type + p_tmtype, 32 * 32 / 8 )
            || !fread( TEMP_PAL, 16, sizeof( u16 ), TM_ICON_FILE )
            || !fread( TEMP, 32 * 32 / 8, sizeof( u32 ), TM_ICON_FILE ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal,
                               NoItemTiles, NoItemTilesLen, false, false, false,
                               p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }

        return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP,
                           512, false, false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0,
                           p_bottom );
    }

    u16 loadTMIconB( BATTLE::type p_type, u8 p_tmtype, const s16 p_posX, const s16 p_posY,
                     u8 p_oamIdx, u16 p_tileCnt, bool p_bottom ) {
        if( !FS::checkOrOpen( TM_ICON_FILE, TM_PATH )
            || !seekSpriteData( TM_ICON_FILE, 3 * p_type + p_tmtype, 32 * 32 / 8 )
            || !fread( TEMP_PAL, 16, sizeof( u16 ), TM_ICON_FILE )
            || !fread( TEMP, 32 * 32 / 8, sizeof( u32 ), TM_ICON_FILE ) ) {
            return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, NoItemPal, NoItemTiles,
                                NoItemTilesLen, false, false, false,
                                p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
        }
        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL, TEMP, 512, false,
                            false, false, p_bottom ? OBJPRIORITY_1 : OBJPRIORITY_0, p_bottom );
    }

    FILE* TYPE_ICON_FILE          = nullptr;
    u8    LAST_TYPE_ICON_LANGUAGE = 0;

    u16 loadTypeIcon( BATTLE::type p_type, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                      u8 p_palIdx, u16 p_tileCnt, bool p_bottom, const SAVE::language p_language ) {
        if( !FS::checkOrOpen( TYPE_ICON_FILE, TYPE_ICON_PATH, LAST_TYPE_ICON_LANGUAGE,
                              p_language ) ) {
            return false;
        }
        if( !seekSpriteData( TYPE_ICON_FILE, p_type, 16 * 32 / 8 ) ) { return false; }
        if( !fread( TEMP, 16 * 32 / 8, sizeof( u32 ), TYPE_ICON_FILE ) ) { return false; }
        if( !fread( TEMP_PAL, 16, sizeof( u16 ), TYPE_ICON_FILE ) ) { return false; }

        return loadSprite( p_oamIdx, p_palIdx, p_tileCnt, p_posX, p_posY, 32, 16, TEMP_PAL, TEMP,
                           16 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadTypeIconB( BATTLE::type p_type, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                       u16 p_tileCnt, bool p_bottom, const SAVE::language p_language ) {
        if( !FS::checkOrOpen( TYPE_ICON_FILE, TYPE_ICON_PATH, LAST_TYPE_ICON_LANGUAGE,
                              p_language ) ) {
            return false;
        }
        if( !seekSpriteData( TYPE_ICON_FILE, p_type, 16 * 32 / 8 ) ) { return false; }
        if( !fread( TEMP, 16 * 32 / 8, sizeof( u32 ), TYPE_ICON_FILE ) ) { return false; }
        if( !fread( TEMP_PAL, 16, sizeof( u16 ), TYPE_ICON_FILE ) ) { return false; }

        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 16, TEMP_PAL, TEMP,
                            16 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
    }

    FILE* CONTEST_TYPE_ICON_FILE          = nullptr;
    u8    LAST_CONTEST_TYPE_ICON_LANGUAGE = 0;

    u16 loadContestTypeIcon( BATTLE::contestType p_type, const s16 p_posX, const s16 p_posY,
                             u8 p_oamIdx, u8 p_palIdx, u16 p_tileCnt, bool p_bottom,
                             const SAVE::language p_language ) {
        if( !FS::checkOrOpen( CONTEST_TYPE_ICON_FILE, CONTEST_TYPE_ICON_PATH,
                              LAST_CONTEST_TYPE_ICON_LANGUAGE, p_language ) ) {
            return false;
        }
        if( !seekSpriteData( CONTEST_TYPE_ICON_FILE, p_type, 16 * 32 / 8 ) ) { return false; }
        if( !fread( TEMP, 16 * 32 / 8, sizeof( u32 ), CONTEST_TYPE_ICON_FILE ) ) { return false; }
        if( !fread( TEMP_PAL, 16, sizeof( u16 ), CONTEST_TYPE_ICON_FILE ) ) { return false; }

        return loadSprite( p_oamIdx, p_palIdx, p_tileCnt, p_posX, p_posY, 32, 16, TEMP_PAL, TEMP,
                           16 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadContestTypeIconB( BATTLE::contestType p_type, const s16 p_posX, const s16 p_posY,
                              u8 p_oamIdx, u16 p_tileCnt, bool p_bottom,
                              const SAVE::language p_language ) {
        if( !FS::checkOrOpen( CONTEST_TYPE_ICON_FILE, CONTEST_TYPE_ICON_PATH,
                              LAST_CONTEST_TYPE_ICON_LANGUAGE, p_language ) ) {
            return false;
        }
        if( !seekSpriteData( CONTEST_TYPE_ICON_FILE, p_type, 16 * 32 / 8 ) ) { return false; }
        if( !fread( TEMP, 16 * 32 / 8, sizeof( u32 ), CONTEST_TYPE_ICON_FILE ) ) { return false; }
        if( !fread( TEMP_PAL, 16, sizeof( u16 ), CONTEST_TYPE_ICON_FILE ) ) { return false; }

        return loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 16, TEMP_PAL, TEMP,
                            16 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadLocationBackB( u8 p_idx, const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
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
    u16 loadPlatform( u8 p_platform, const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                      u16 p_tileCnt, bool p_bottom ) {
        snprintf( BUFFER, 99, "plat%hhu", p_platform );
        if( FS::readData<unsigned short, unsigned int>( "nitro:/PICS/SPRITES/PLAT/", BUFFER, 16,
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

    u16 loadRibbonIcon( u8 p_ribbonIdx, const s16 p_posX, const s16 p_posY, u8 p_oamIdx,
                        u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {

        if( FS::readData( "nitro:/PICS/SPRITES/RIBBON/",
                          ( "r" + std::to_string( p_ribbonIdx ) ).c_str( ),
                          (unsigned int) 32 * 32 / 8, TEMP, (unsigned short) 16, TEMP_PAL ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL,
                               TEMP, 32 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
        }
        return p_tileCnt + ( 32 * 16 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 loadShapeIcon( u8 p_shapeIdx, const s16 p_posX, const s16 p_posY, u8 p_oamIdx, u8 p_palCnt,
                       u16 p_tileCnt, bool p_bottom ) {

        if( FS::readData( "nitro:/PICS/SPRITES/SHAPES/", std::to_string( p_shapeIdx ).c_str( ),
                          (unsigned int) 32 * 32 / 8, TEMP, (unsigned short) 16, TEMP_PAL ) ) {
            return loadSprite( p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 32, TEMP_PAL,
                               TEMP, 32 * 32 / 2, false, false, false, OBJPRIORITY_0, p_bottom );
        }
        return p_tileCnt + ( 32 * 16 ) / BYTES_PER_16_COLOR_TILE;
    }

    u16 loadDamageCategoryIcon( BATTLE::moveHitTypes p_type, const s16 p_posX, const s16 p_posY,
                                u8 p_oamIdx, u8 p_palCnt, u16 p_tileCnt, bool p_bottom ) {
        char buffer[ 30 ];
        snprintf( buffer, 29, "BT/damage_%hhu", u8( p_type ) - 1 );
        return loadSprite( buffer, p_oamIdx, p_palCnt, p_tileCnt, p_posX, p_posY, 32, 16, false,
                           false, false, OBJPRIORITY_0, p_bottom );
    }

    u16 loadDamageCategoryIconB( BATTLE::moveHitTypes p_type, const s16 p_posX, const s16 p_posY,
                                 u8 p_oamIdx, u16 p_tileCnt, bool p_bottom ) {
        char buffer[ 30 ];
        snprintf( buffer, 29, "BT/damage_%hhu", u8( p_type ) - 1 );
        return loadSpriteB( buffer, p_oamIdx, p_tileCnt, p_posX, p_posY, 32, 16, false, false,
                            false, OBJPRIORITY_0, p_bottom );
    }

} // namespace IO
