/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : sprite.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Some sprite code.

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


#include "sprite.h"
#include "berry.h"

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


//deprecated
void initTypes( OAMTable * p_oam, SpriteInfo *p_spriteInfo, Type p_T1, Type p_T2 ) {

    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
        sizeof( SPRITE_GFX_SUB[ 0 ] );

    /* Keep track of the available tiles */
    int nextAvailableTileIdx = 0;

    static const int TYPE_1_ID = 0;
    SpriteInfo * type1Info = &p_spriteInfo[ TYPE_1_ID ];
    SpriteEntry * type1 = &p_oam->oamBuffer[ TYPE_1_ID ];
    type1Info->oamId = TYPE_1_ID;
    type1Info->width = 32;
    type1Info->height = 16;
    type1Info->angle = 0;
    type1Info->entry = type1;
    type1->y = 5;
    type1->isRotateScale = false;
    type1->isHidden = false;
    type1->blendMode = OBJMODE_NORMAL;
    type1->isMosaic = false;
    type1->colorMode = OBJCOLOR_16;
    type1->shape = OBJSHAPE_WIDE;
    type1->x = SCREEN_WIDTH - 5 - type1Info->width - ( p_T1 != p_T2 ? ( 2 + type1Info->width ) : 0 );
    type1->size = OBJSIZE_32;
    type1->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += KampfTilesLen / BYTES_PER_16_COLOR_TILE;
    type1->priority = OBJPRIORITY_0;
    type1->palette = type1Info->oamId;

    static const int TYPE_2_ID = 1;
    SpriteInfo * type2Info = &p_spriteInfo[ TYPE_2_ID ];
    SpriteEntry * type2 = &p_oam->oamBuffer[ TYPE_2_ID ];

    if( p_T1 != p_T2 ) {
        type2Info->oamId = TYPE_2_ID;
        type2Info->width = 32;
        type2Info->height = 16;
        type2Info->angle = 0;
        type2Info->entry = type2;
        type2->y = 5;
        type2->isRotateScale = false;
        type2->isHidden = false;
        type2->blendMode = OBJMODE_NORMAL;
        type2->isMosaic = false;
        type2->colorMode = OBJCOLOR_16;
        type2->shape = OBJSHAPE_WIDE;
        type2->x = SCREEN_WIDTH - 5 - type2Info->width;
        type2->size = OBJSIZE_32;
        type2->gfxIndex = nextAvailableTileIdx;
        nextAvailableTileIdx += KampfTilesLen / BYTES_PER_16_COLOR_TILE;
        type2->priority = OBJPRIORITY_0;
        type2->palette = type2Info->oamId;
    };

    /*************************************************************************/

    /* Copy over the sprite palettes */

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      TypePals[ p_T1 ],
                      &SPRITE_PALETTE_SUB[ type1Info->oamId * COLORS_PER_PALETTE ],
                      COLORS_PER_PALETTE * 2 );
    if( p_T1 != p_T2 )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
        TypePals[ p_T2 ],
        &SPRITE_PALETTE_SUB[ type2Info->oamId * COLORS_PER_PALETTE ],
        COLORS_PER_PALETTE * 2 );
    /* Copy the sprite graphics to sprite graphics memory */
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      TypeTiles[ p_T1 ],
                      &SPRITE_GFX_SUB[ type1->gfxIndex * OFFSET_MULTIPLIER ],
                      KampfTilesLen );
    if( p_T1 != p_T2 )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
        TypeTiles[ p_T2 ],
        &SPRITE_GFX_SUB[ type2->gfxIndex * OFFSET_MULTIPLIER ],
        KampfTilesLen );
}