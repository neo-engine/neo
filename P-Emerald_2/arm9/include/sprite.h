/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : sprite.h
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

static const int SPRITE_DMA_CHANNEL = 2;

struct SpriteInfo {
    int oamId;
    int width;
    int height;
    int angle;
    SpriteEntry * entry;
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
void rotateSprite( SpriteRotation * p_spriteRotation, int p_angle );

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

//Some specific things
void initTypes( OAMTable * p_oam, SpriteInfo *p_spriteInfo, Type p_T1, Type p_T2 );
int initMainSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo );