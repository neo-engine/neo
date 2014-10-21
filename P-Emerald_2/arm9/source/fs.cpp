/*
Pokémon Emerald 2 Version
------------------------------

file        : fs.cpp
author      : Philip Wellnitz (RedArceus)
description : Filesystem interactions

Copyright (C) 2012 2014
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


#include <string>

#include "fs.h"
#include "buffer.h"
#include "ability.h"
#include "pokemon.h"
#include "item.h"
#include "berry.h"


u32 TEMP[ 12288 ] = { 0 };
u16 TEMP_PAL[ 256 ] = { 0 };
const char ITEM_PATH[ ] = "nitro:/ITEMS/";
const char PKMNDATA_PATH[ ] = "nitro:/PKMNDATA/";
const char ABILITYDATA_PATH[ ] = "nitro:/PKMNDATA/ABILITIES/";

ability::ability( int p_abilityId ) {
    sprintf( buffer, "nitro:/LOCATIONS/%i.data", p_abilityId );
    FILE* f = fopen( buffer, "r" );

    if( !f )
        return;

    m_abilityName = FS::readString( f, false );
    m_flavourText = FS::readString( f, false );
    fscanf( f, "%u", &( m_type ) );
    fclose( f );
}

std::wstring getWAbilityName( int p_abilityId ) {
    sprintf( buffer, "nitro:/LOCATIONS/%i.data", p_abilityId );
    FILE* f = fopen( buffer, "r" );

    if( !f )
        return L"---";
    auto ret = FS::readWString( f, false );
    fclose( f );
    return ret;
}

namespace FS {
    bool loadSprite( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt ) {
        //char buffer[100];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fread( TEMP, sizeof( u32 ), p_tileCnt, fd );
        fread( TEMP_PAL, sizeof( u16 ), p_palCnt, fd );

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_spriteInfo->m_entry->gfxIndex * OFFSET_MULTIPLIER ], 4 * p_tileCnt );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ p_spriteInfo->m_entry->palette * COLORS_PER_PALETTE ], 2 * p_palCnt );
        fclose( fd );
        return true;
    }
    bool loadSpriteSub( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt ) {
        //char buffer[100];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fread( TEMP, sizeof( u32 ), p_tileCnt, fd );
        fread( TEMP_PAL, sizeof( u16 ), p_palCnt, fd );

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          TEMP,
                          &SPRITE_GFX_SUB[ p_spriteInfo->m_entry->gfxIndex * OFFSET_MULTIPLIER ],
                          4 * p_tileCnt );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          TEMP_PAL,
                          &SPRITE_PALETTE_SUB[ p_spriteInfo->m_entry->palette * COLORS_PER_PALETTE ],
                          2 * p_palCnt );
        fclose( fd );
        return true;
    }

    bool loadPKMNSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const u16& p_pkmnId, const u16 p_posX,
                         const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_tileCnt, bool p_bottom, bool p_shiny, bool p_female, bool p_flipx ) {
        //char buffer[100];
        if( !p_female )
            sprintf( buffer, "%s%d/%d.raw", p_path, p_pkmnId, p_pkmnId );
        else
            sprintf( buffer, "%s%d/%df.raw", p_path, p_pkmnId, p_pkmnId );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        //p_palCnt = 16;
        for( int i = 0; i < 16; ++i )
            TEMP_PAL[ i ] = 0;
        fread( TEMP_PAL, sizeof( u16 ), 16, fd );
        for( int i = 0; i < 96 * 96; ++i )
            TEMP[ i ] = 0;
        fread( TEMP, sizeof( u32 ), 96 * 96, fd );
        fclose( fd );
        if( p_shiny ) {
            memset( buffer, 0, sizeof( buffer ) );
            if( !p_female )
                sprintf( buffer, "%s%d/%ds.raw", p_path, p_pkmnId, p_pkmnId );
            else
                sprintf( buffer, "%s%d/%dsf.raw", p_path, p_pkmnId, p_pkmnId );
            fd = fopen( buffer, "rb" );
            for( int i = 0; i < 16; ++i )
                TEMP_PAL[ i ] = 0;
            fread( TEMP_PAL, sizeof( u16 ), 16, fd );
            fclose( fd );
        }
        if( p_bottom ) {
            //swiCopy(TEMP_PAL,&SPRITE_PALETTE_SUB[(++p_palCnt) * COLORS_PER_PALETTE], 32 | COPY_MODE_HWORD);
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
        } else {
            //swiCopy(TEMP_PAL,&SPRITE_PALETTE[(++p_palCnt) * COLORS_PER_PALETTE], 32 | COPY_MODE_HWORD);
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
        }
        SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 64;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = p_flipx ? 32 + p_posX : p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        if( p_bottom ) {
            //swiCopy(TEMP,&SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER], 96*96/2 | COPY_MODE_HWORD);
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        } else {
            //swiCopy(TEMP,&SPRITE_GFX[p_tileCnt * OFFSET_MULTIPLIER], 96*96/2 | COPY_MODE_HWORD);
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        }
        p_tileCnt += 64;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_TALL;
        back->x = p_flipx ? p_posX : 64 + p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 32;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 64;
        backInfo->m_height = 32;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY + 64;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_WIDE;
        back->x = p_flipx ? 32 + p_posX : p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 32;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 32;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY + 64;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = p_flipx ? p_posX : 64 + p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_32;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 16;

        ++p_palCnt;
        if( p_bottom )
            updateOAMSub( p_oam );
        else
            updateOAM( p_oam );
        return true;
    }
    bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const u16& p_pkmnId, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_tileCnt, bool p_bottom, bool p_shiny, bool p_female, bool p_flipx ) {

        if( !p_female )
            sprintf( buffer, "%s%d/%d.raw", p_path, p_pkmnId, p_pkmnId );
        else
            sprintf( buffer, "%s%d/%df.raw", p_path, p_pkmnId, p_pkmnId );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        //p_palCnt = 16;
        for( int i = 0; i < 16; ++i )
            TEMP_PAL[ i ] = 0;
        fread( TEMP_PAL, sizeof( u16 ), 16, fd );
        for( int i = 0; i < 96 * 96; ++i )
            TEMP[ i ] = 0;
        fread( TEMP, sizeof( u32 ), 96 * 64, fd );
        fclose( fd );
        if( p_shiny ) {
            memset( buffer, 0, sizeof( buffer ) );
            if( !p_female )
                sprintf( buffer, "%s%d/%ds.raw", p_path, p_pkmnId, p_pkmnId );
            else
                sprintf( buffer, "%s%d/%dsf.raw", p_path, p_pkmnId, p_pkmnId );
            fd = fopen( buffer, "rb" );
            for( int i = 0; i < 16; ++i )
                TEMP_PAL[ i ] = 0;
            fread( TEMP_PAL, sizeof( u16 ), 16, fd );
            fclose( fd );
        }
        if( p_bottom )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
        else
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );

        SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 64;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = p_flipx ? 32 + p_posX : p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        if( p_bottom )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        else
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        p_tileCnt += 64;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_TALL;
        back->x = p_flipx ? p_posX : 64 + p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 32;

        ++p_palCnt;
        if( p_bottom )
            updateOAMSub( p_oam );
        else
            updateOAM( p_oam );
        return true;
    }

    bool loadTrainerSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_tileCnt, bool p_bottom, bool p_flipx ) {
        //char buffer[100];

        sprintf( buffer, "%sSprite_%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        //p_palCnt = 16;
        for( int i = 0; i < 16; ++i )
            TEMP_PAL[ i ] = 0;
        fread( TEMP_PAL, sizeof( u16 ), 16, fd );
        for( int i = 0; i < 96 * 96; ++i )
            TEMP[ i ] = 0;
        fread( TEMP, sizeof( u32 ), 96 * 96, fd );
        fclose( fd );

        if( p_bottom )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
        else
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );

        SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 64;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = p_flipx ? 32 + p_posX : p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        if( p_bottom )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        else
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        p_tileCnt += 64;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_TALL;
        back->x = p_flipx ? p_posX : 64 + p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 32;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 64;
        backInfo->m_height = 32;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY + 64;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_WIDE;
        back->x = p_flipx ? 32 + p_posX : p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 32;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 32;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY + 64;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = p_flipx ? p_posX : 64 + p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_32;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 16;

        ++p_palCnt;
        if( p_bottom )
            updateOAMSub( p_oam );
        else
            updateOAM( p_oam );
        return true;
    }
    bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u16 p_posX,
                            const u16 p_posY, u8& p_oamIndex, u8& p_palCnt, u16& p_tileCnt, bool p_bottom, bool p_flipx ) {

        sprintf( buffer, "%sSprite_%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        //p_palCnt = 16;
        for( int i = 0; i < 16; ++i )
            TEMP_PAL[ i ] = 0;
        fread( TEMP_PAL, sizeof( u16 ), 16, fd );
        for( int i = 0; i < 96 * 96; ++i )
            TEMP[ i ] = 0;
        fread( TEMP, sizeof( u32 ), 96 * 64, fd );
        fclose( fd );

        if( p_bottom )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
        else
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );

        SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 64;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = p_flipx ? 32 + p_posX : p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        if( p_bottom )
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        else
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
        p_tileCnt += 64;

        backInfo = &p_spriteInfo[ ++p_oamIndex ];
        back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 64;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = p_posY;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_TALL;
        back->x = p_flipx ? p_posX : 64 + p_posX;
        back->hFlip = p_flipx;
        back->size = OBJSIZE_64;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = p_palCnt;
        p_tileCnt += 32;

        ++p_palCnt;
        if( p_bottom )
            updateOAMSub( p_oam );
        else
            updateOAM( p_oam );
        return true;
    }

    bool loadPicture( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize, u32 p_tileCnt ) {

        //char buffer[100];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }

        fread( TEMP, sizeof( u32 ), 12288, fd );
        fread( TEMP_PAL, sizeof( u16 ), 256, fd );

        dmaCopy( TEMP, p_layer, p_tileCnt );
        dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
        fclose( fd );

        return true;
    }
    bool loadPictureSub( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize, u32 p_tileCnt ) {

        //char buffer[100];
        sprintf( buffer, "%s%s.raw", p_path, p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }

        fread( TEMP, sizeof( u32 ), 12288, fd );
        fread( TEMP_PAL, sizeof( u16 ), 256, fd );

        dmaCopy( TEMP, p_layer, p_tileCnt );
        dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
        fclose( fd );


        return true;
    }

    bool loadNavScreen( u16* p_layer, const char* p_name, u8 p_no ) {
        if( p_no == BG_ind && NAV_DATA[ 0 ] ) {
            dmaCopy( NAV_DATA, p_layer, 256 * 192 );
            dmaCopy( NAV_DATA_PAL, BG_PALETTE_SUB, 256 * 2 );
            return true;
        }

        //char buffer[100];
        sprintf( buffer, "nitro:/PICS/NAV/%s.raw", p_name );
        FILE* fd = fopen( buffer, "rb" );

        if( fd == 0 ) {
            fclose( fd );
            return false;
        }

        fread( NAV_DATA, sizeof( u32 ), 12288, fd );
        fread( NAV_DATA_PAL, sizeof( u16 ), 256, fd );

        dmaCopy( NAV_DATA, p_layer, 256 * 192 );
        dmaCopy( NAV_DATA_PAL, BG_PALETTE_SUB, 256 * 2 );
        fclose( fd );

        return true;
    }

    std::string readString( FILE* p_file, bool p_new ) {
        std::string ret = "";
        char ac;

        while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );

        if( ac == '*' ) {
            ret += '\0';
            return ret;
        } else ret += ac;

        while( ( ac = fgetc( p_file ) ) != '*' ) {
            if( !p_new ) {
                if( ac == 'ä' )
                    ret += '\x84';
                else if( ac == 'Ä' )
                    ret += '\x8E';
                else if( ac == 'ü' )
                    ret += '\x81';
                else if( ac == 'Ü' )
                    ret += '\x9A';
                else if( ac == 'ö' )
                    ret += '\x94';
                else if( ac == 'Ö' )
                    ret += '\x99';
                else if( ac == 'ß' )
                    ret += '\x9D';
                else if( ac == 'é' )
                    ret += '\x82';
                else if( ac == '%' )
                    ret += ' ';
                else if( ac == '|' )
                    ret += (char)136;
                else if( ac == '#' )
                    ret += (char)137;
                else
                    ret += ac;
                continue;
            }
            if( ac == '|' )
                ret += (char)136;
            else if( ac == '#' )
                ret += (char)137;
            else
                ret += ac;
        }
        ret += '\0';
        return ret;
    }

    std::wstring readWString( FILE* p_file, bool p_new ) {
        std::wstring ret = L"";
        char ac;
        while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );
        if( ac == '*' ) {
            ret += L'\0';
            return ret;
        } else ret += ac;
        while( ( ac = fgetc( p_file ) ) != '*' ) {
            if( !p_new ) {
                if( ac == 'ä' )
                    ret += '\x84';
                else if( ac == 'Ä' )
                    ret += '\x8E';
                else if( ac == 'ü' )
                    ret += '\x81';
                else if( ac == 'Ü' )
                    ret += '\x9A';
                else if( ac == 'ö' )
                    ret += '\x94';
                else if( ac == 'Ö' )
                    ret += '\x99';
                else if( ac == 'ß' )
                    ret += '\x9D';
                else if( ac == 'é' )
                    ret += '\x82';
                else if( ac == '%' )
                    ret += ' ';
                else
                    ret += ac;
                continue;
            }
            if( ac == '|' )
                ret += (char)136;
            else if( ac == '#' )
                ret += (char)137;
            else
                ret += ac;
        }
        ret += L'\0';
        return ret;
    }

    const char* getLoc( u16 p_ind ) {
        if( p_ind < 0 || p_ind > 5000 )
            return "Entfernter Ort";

        sprintf( buffer, "nitro:/LOCATIONS/%i.data", p_ind );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 ) {
            fclose( f );
            if( savMod == SavMod::_NDS && p_ind > 322 && p_ind < 1000 )
                return getLoc( 3002 );

            return "Entfernter Ort";
        }
        std::string ret = readString( f );
        fclose( f );
        return ret.c_str( );
    }
}

namespace POKEMON {
    namespace PKMNDATA {
        Type getType( u16 p_pkmnId, u16 p_type ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return UNBEKANNT;
            }
            char buf[ 12 ];
            fscanf( f, "%s", buf );
            fclose( f );
            return (Type)( buf[ p_type ] - 42 );
        }
        u16 getBase( u16 p_pkmnId, u16 p_base ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return 0;
            }
            char buf[ 12 ];
            fscanf( f, "%s", buf );
            fclose( f );
            return (short)buf[ 2 + p_base ];
        }
        u16 getCatchRate( u16 p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return 0;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fclose( f );
            return buf;
        }
        const char* getDisplayName( u16 p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return "???";
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            std::string ret = FS::readString( f, true );
            fclose( f );
            return ret.c_str( );
        }
        const wchar_t* getWDisplayName( u16 p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return L"???";
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            std::wstring ret = FS::readWString( f );
            fclose( f );
            return ret.c_str( );
        }
        void getWDisplayName( u16 p_pkmnId, wchar_t* p_name ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                wcscpy( p_name, L"???" );
                return;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            std::wstring ret = FS::readWString( f );
            fclose( f );
            wcscpy( p_name, ret.c_str( ) );
        }
        void getHoldItems( u16 p_pkmnId, u16* p_items ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                p_items[ 0 ] = p_items[ 1 ] = p_items[ 2 ] = p_items[ 3 ] = 0;
                return;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            FS::readString( f );
            p_items[ 0 ] = p_items[ 1 ] = p_items[ 2 ] = p_items[ 3 ] = 0;
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &p_items[ i ] );
            fclose( f );
            return;
        }
        pkmnGenderType getGenderType( u16 p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return (pkmnGenderType)0;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            FS::readString( f );
            for( int i = 0; i < 5; ++i )
                fscanf( f, "%hi", &buf );
            fclose( f );
            return (pkmnGenderType)buf;
        }
        const char* getDexEntry( u16 p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return "Keine Daten vorhanden.";
            }
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            for( int i = 2; i < 8; ++i )
                fgetc( f );
            short s;
            fscanf( f, "%hi", &s );
            fgetc( f );
            FS::readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &s );
            fscanf( f, "%hi", &s );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            fgetc( f );
            fgetc( f );
            for( int i = 0; i < 6; ++i )
                fgetc( f );
            fscanf( f, "%hi", &s );
            fgetc( f );
            std::string ret = FS::readString( f );
            fclose( f );
            return ret.c_str( );
        }
        u16 getForme( u16 p_pkmnId, u16 p_formeId, const char* p_retFormeName ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return p_pkmnId;
            }
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            for( int i = 2; i < 8; ++i )
                fgetc( f );
            u16 s;
            fscanf( f, "%hi", &s );
            fgetc( f );
            FS::readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &s );
            fscanf( f, "%hi", &s );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            fgetc( f );
            fgetc( f );
            for( int i = 0; i < 6; ++i )
                fgetc( f );
            fscanf( f, "%hi", &s );
            fgetc( f );
            FS::readString( f );
            fscanf( f, "%hi", &s );
            u16 d; std::string ret;
            for( int i = 0; i <= std::min( s, p_formeId ); ++i ) {
                fscanf( f, "%hi", &d ); ret = FS::readString( f );
            }
            //fscanf(f,"%hi",d); 
            //ret = readString(f);
            fclose( f );
            p_retFormeName = ret.c_str( );
            return d;
        }
        const char* getSpecies( u16 p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return "UNBEKANNT";
            }
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            for( int i = 2; i < 8; ++i )
                fgetc( f );
            short s;
            fscanf( f, "%hi", &s );
            fgetc( f );
            FS::readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &s );
            fscanf( f, "%hi", &s );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            fgetc( f );
            fgetc( f );
            for( int i = 0; i < 6; ++i )
                fgetc( f );
            fscanf( f, "%hi", &s );
            fgetc( f );
            FS::readString( f );
            fscanf( f, "%hi", &s );
            short d; std::string ret2;
            for( int i = 0; i < s; ++i ) {
                fscanf( f, "%hi", &d ); FS::readString( f );
            }
            fscanf( f, " " );
            ret2 = FS::readString( f, true );
            fclose( f );
            return ret2.c_str( );
        }

        void getAll( u16 p_pkmnId, pokemonData& p_out ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return;
            }
            for( int i = 0; i < 2; ++i )
                p_out.m_types[ i ] = (Type)( ( fgetc( f ) ) - 42 );
            for( int i = 2; i < 8; ++i )
                p_out.m_bases[ i - 2 ] = (short)fgetc( f );
            fscanf( f, "%hi", &p_out.m_catchrate );
            fgetc( f );
            FS::readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &p_out.m_items[ i ] );
            fscanf( f, "%hi", &p_out.m_gender );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                p_out.m_eggT[ i ] = (pkmnEggType)( ( fgetc( f ) ) - 42 );
            p_out.m_eggcyc = ( ( fgetc( f ) ) - 42 );
            p_out.m_baseFriend = ( ( fgetc( f ) ) - 42 );
            for( int i = 0; i < 6; ++i )
                p_out.m_EVYield[ i ] = ( ( fgetc( f ) ) - 42 );
            fscanf( f, "%hi", &p_out.m_EXPYield );
            fgetc( f );
            FS::readString( f );
            fscanf( f, "%hi", &p_out.m_formecnt );
            short d;
            for( int i = 0; i < p_out.m_formecnt; ++i ) {
                fscanf( f, "%hi", &d ); FS::readString( f );
            }
            fscanf( f, " " );
            FS::readString( f, true );
            fscanf( f, "%hi", &p_out.m_size );
            fscanf( f, "%hi", &p_out.m_weight );
            fscanf( f, "%hi", &p_out.m_expType );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hu ", &p_out.m_abilities[ i ] );
            for( int i = 0; i < 7; ++i )
                for( int j = 0; j < 15; ++j )
                    fscanf( f, "%hi ", &( p_out.m_evolutions[ i ].m_evolveData[ j ] ) );
            fclose( f );
            return;
        }

        void getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_mode, u16 p_amount, u16* p_result ) {

            char pt[ 150 ];
            sprintf( pt, "%s/LEARNSETS/%d.learnset.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );
            if( !f )
                return;

            int rescnt = 0;

            if( p_fromLevel > p_toLevel ) {
                std::vector<u16> reses;
                for( int i = 0; i <= p_fromLevel; ++i ) {
                    int z; fscanf( f, "%d", &z );
                    for( int j = 0; j < z; ++j ) {
                        u16 g, h;
                        fscanf( f, "%hd %hd", &g, &h );
                        if( i >= p_toLevel && h == (u16)p_mode && g < MAXATTACK )
                            reses.push_back( g );
                    }
                }
                auto I = reses.rbegin( );
                for( int i = 0; i < p_amount && I != reses.rend( ); ++i, ++I ) {
                    for( int z = 0; z < i; ++z )
                        if( *I == p_result[ z ] ) {
                        --i;
                        goto N;
                        }
                    p_result[ i ] = *I;
N:
                    ;
                }
                fclose( f );
                return;
            } else {
                for( int i = 0; i <= p_toLevel; ++i ) {
                    int z; fscanf( f, "%d", &z );
                    for( int j = 0; j < z; ++j ) {
                        u16 g, h;
                        fscanf( f, "%hd %hd", &g, &h );
                        if( i >= p_fromLevel && h == p_mode && g < MAXATTACK ) {
                            for( int z = 0; z < rescnt; ++z )
                                if( g == p_result[ z ] )
                                    goto NEXT;
                            p_result[ rescnt ] = g;
                            if( ++rescnt == p_amount )
                                return;
NEXT:
                            ;
                        }
                    }
                }
            }
            fclose( f );
        }
        bool canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_mode ) {
            char pt[ 150 ];
            sprintf( pt, "%s/LEARNSETS/%d.learnset.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );
            if( !f )
                return false;

            for( int i = 0; i <= 100; ++i ) {
                int z; fscanf( f, "%d", &z );
                for( int j = 0; j < z; ++j ) {
                    u16 g, h;
                    fscanf( f, "%hd %hd", &g, &h );
                    if( g == p_moveId && h == p_mode )
                        return true;
                }
            }
            return false;
        }

        u16 getColor( Type p_type ) {
            switch( p_type ) {
                case NORMAL:
                    return NORMAL_;
                    break;
                case KAMPF:
                    return RED;
                    break;
                case FLUG:
                    return TURQOISE;
                    break;
                case GIFT:
                    return POISON;
                    break;
                case BODEN:
                    return GROUND;
                    break;
                case GESTEIN:
                    return ROCK;
                    break;
                case KAEFER:
                    return BUG;
                    break;
                case GEIST:
                    return GHOST;
                    break;
                case STAHL:
                    return STEEL;
                    break;
                case UNBEKANNT:
                    return UNKNOWN;
                    break;
                case WASSER:
                    return BLUE;
                    break;
                case FEUER:
                    return ORANGE;
                    break;
                case PFLANZE:
                    return GREEN;
                    break;
                case ELEKTRO:
                    return YELLOW;
                    break;
                case PSYCHO:
                    return PURPLE;
                    break;
                case EIS:
                    return ICE;
                    break;
                case DRACHE:
                    return DRAGON;
                    break;
                case UNLICHT:
                    return BLACK;
                    break;
                case FEE:
                    return FAIRY;
                    break;
                default:
                    return DRAGON;
                    break;
            }
            return WHITE;
        }
    }
}

namespace ITEMS {
    u16 item::getItemId( ) {
        for( int i = 0; i < 700; ++i )
            if( ItemList[ i ].m_itemName == m_itemName )
                return i;
        return 0;
    }

    bool item::load( ) {
        //    std::stringstream FILENAME;
        //    FILENAME << ITEM_PATH << Name << ".data";
        //    FILE* f = fopen(FILENAME.str().c_str(),"r");
        //    if(f == 0)
        //        return false;
        //    //itemtype = GOODS;
        //    int ac;
        //    fscanf(f,"%i",&ac);
        //    effekt = item::EFFEKT(ac);
        //    fscanf(f,"%i\n",&(price));
        //    displayName = readString(f);
        //    dscrpt = readString(f);
        //    effekt_script = readString(f);
        //    fclose(f);
        return m_loaded = true;
    }

    bool berry::load( ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );

        if( f == 0 )
            return m_loaded = false;
        //itemtype = BERRIES;
        int ac;
        fscanf( f, "%i", &ac );
        //effekt = item::EFFEKT(ac);
        //fscanf(f,"%i\n",&(price));
        fscanf( f, "%i", &ac );
        /*displayName = */FS::readString( f, false );
        /*dscrpt = "  "+ */FS::readString( f, false );
        /*effekt_script = */FS::readString( f, false );

        fscanf( f, "%hi", &( m_berrySize ) );

        fscanf( f, "%i", &ac );
        m_berryGuete = berry::berryGueteType( ac );

        fscanf( f, "%i", &ac );
        m_naturalGiftType = Type( ac );

        fscanf( f, "%hhu", &( m_naturalGiftStrength ) );

        for( int i = 0; i < 5; ++i )
            fscanf( f, "%hhu", &( m_berryTaste[ i ] ) );

        fscanf( f, "%hhu", &( m_hoursPerGrowthStage ) );

        fscanf( f, "%hhu", &( m_minBerries ) );

        fscanf( f, "%hhu\n", &( m_maxBerries ) );

        fclose( f );
        return m_loaded = true;
    }

    std::string item::getDescription( bool p_new ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 )
            return "Keine Daten.";
        int ac;
        fscanf( f, "%i", &ac );
        fscanf( f, "%i\n", &ac );
        std::string s = FS::readString( f, p_new );
        s = FS::readString( f, p_new );
        fclose( f );
        return s;
    }

    std::string item::getDisplayName( bool p_new ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 )
            return m_itemName;
        int ac;
        fscanf( f, "%i", &ac );
        fscanf( f, "%i\n", &ac );
        std::string s = FS::readString( f, p_new );
        fclose( f );
        return s;
    }

    item::itemEffectType item::getEffectType( ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 )
            return ( item::itemEffectType::NONE );
        int ac;
        fscanf( f, "%i", &ac );
        fclose( f );
        return ( item::itemEffectType )ac;
    }

    item::itemType item::getItemType( ) {
        return m_itemType;
    }

    u32 item::getPrice( ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 )
            return 0;
        int ac;
        fscanf( f, "%i", &ac );
        fscanf( f, "%i\n", &ac );
        fclose( f );
        return ac;
    }

    std::string berry::getDescription2( bool p_new ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 )
            return "Keine Daten.";
        //itemtype = BERRIES;
        int ac;
        fscanf( f, "%i", &ac );
        //effekt = item::EFFEKT(ac);
        //fscanf(f,"%i\n",&(price));
        fscanf( f, "%i", &ac );
        /*displayName = */FS::readString( f, p_new );
        /*dscrpt = "  "+ */FS::readString( f, p_new );
        /*effekt_script = */FS::readString( f, p_new );

        fscanf( f, "%hi", &( ac ) );
        fscanf( f, "%i", &ac );
        //Guete = berry::Guete_Type(ac);
        fscanf( f, "%i", &ac );
        //BeerenKr_Type = Type(ac);
        fscanf( f, "%hhu", &( ac ) );
        for( int i = 0; i < 5; ++i )
            fscanf( f, "%hhu", &( ac ) );
        fscanf( f, "%hhu", &( ac ) );
        fscanf( f, "%hhu", &( ac ) );
        fscanf( f, "%hhu\n", &( ac ) );

        std::string s = FS::readString( f, p_new );
        fclose( f );
        return s;
    }

    std::string item::getShortDescription( bool p_new ) {
        sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
        FILE* f = fopen( buffer, "r" );
        if( f == 0 )
            return "Keine Daten.";
        int ac;
        fscanf( f, "%i", &ac );
        //effekt = item::EFFEKT(ac);
        //fscanf(f,"%i\n",&(price));
        fscanf( f, "%i", &ac );
        /*displayName = */FS::readString( f, p_new );
        /*dscrpt = "  "+ */FS::readString( f, p_new );
        /*effekt_script = */FS::readString( f, p_new );

        if( fscanf( f, "%hi", &( ac ) ) == EOF )
            return "Keine Daten.";
        fscanf( f, "%i", &ac );
        //Guete = berry::Guete_Type(ac);
        fscanf( f, "%i", &ac );
        //BeerenKr_Type = Type(ac);
        fscanf( f, "%hhu", &( ac ) );
        for( int i = 0; i < 5; ++i )
            fscanf( f, "%hhu", &( ac ) );
        fscanf( f, "%hhu", &( ac ) );
        fscanf( f, "%hhu", &( ac ) );
        fscanf( f, "%hhu\n", &( ac ) );

        FS::readString( f, p_new );
        std::string s = FS::readString( f, p_new );
        fclose( f );
        return s;
    }
}