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

extern const unsigned int* TypeTiles[19];
extern const unsigned short* TypePals[19];

static const int SPRITE_DMA_CHANNEL = 2;

struct SpriteInfo{
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
void updateOAM(OAMTable * oam);
void updateOAMSub(OAMTable * oam);

/*
*  initOAM
*
*  Initialize the OAM.
*
*/
void initOAMTable(OAMTable * oam);
void initOAMTableSub(OAMTable * oam);

/*
*  rotateSprite
*
*  Rotate a sprite.
*
*/
void rotateSprite(SpriteRotation * spriteRotation, int angle);

/*
*  setSpriteVisibility
*
*  Hide or show a sprite of a certain type: affine double bound, affine
*  non-double bound, or ordinary.
*
*/
void setSpriteVisibility(SpriteEntry * spriteEntry, bool hidden, bool affine = false, bool doubleBound = false);

inline void setSpritePosition(SpriteEntry* spriteEntry,u16 x = 0,u16 y = 0);
inline void setSpritePriority(SpriteEntry* spriteEntry,ObjPriority priority);

//Some specific things
void initTypes(OAMTable * oam, SpriteInfo *spriteInfo,Type T1, Type T2);
int initMainSprites(OAMTable * oam, SpriteInfo *spriteInfo);