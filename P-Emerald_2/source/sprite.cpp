#include "sprite.h"
#include "berry.h"

const unsigned int* TypeTiles[19] = 
{
    NormalTiles,	KampfTiles, 	FlugTiles,      GiftTiles,	    BodenTiles,
    GestTiles,  	KaeferTiles,    GeistTiles,	    StahlTiles,	    UnbekTiles,
    WasserTiles,	FeuerTiles,	    PflTiles,	    ElekTiles,	    PsychoTiles,
    EisTiles,       DraTiles,	    UnlTiles,       FeeTiles
};
const unsigned short* TypePals[19] = 
{
    NormalPal,	    KampfPal,	    FlugPal,        GiftPal,        BodenPal,
    GestPal,	    KaeferPal,	    GeistPal,	    StahlPal,	    UnbekPal,
    WasserPal,	    FeuerPal,	    PflPal, 	    ElekPal,	    PsychoPal,
    EisPal,         DraPal, 	    UnlPal,         FeePal
};

void updateOAM(OAMTable * oam) {
    DC_FlushAll();
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     oam->oamBuffer,
                     OAM,
                     SPRITE_COUNT * sizeof(SpriteEntry));
}
void updateOAMSub(OAMTable * oam) {
    DC_FlushAll();
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     oam->oamBuffer,
                     OAM_SUB,
                     SPRITE_COUNT * sizeof(SpriteEntry));
}

void initOAMTable(OAMTable * oam) {
    /*
     * For all 128 sprites on the DS, disable and clear any attributes they 
     * might have. This prevents any garbage from being displayed and gives 
     * us a clean slate to work with.
     */
    for (int i = 0; i < SPRITE_COUNT; i++) {
        oam->oamBuffer[i].attribute[0] = ATTR0_DISABLED;
        oam->oamBuffer[i].attribute[1] = 0;
        oam->oamBuffer[i].attribute[2] = 0;
        oam->oamBuffer[i].isHidden = true;
    }
    for (int i = 0; i < MATRIX_COUNT; i++) {
        /* If you look carefully, you'll see this is that affine trasformation
         * matrix again. We initialize it to the identity matrix, as we did
         * with backgrounds
         */
        oam->matrixBuffer[i].hdx = 1 << 8;
        oam->matrixBuffer[i].hdy = 0;
        oam->matrixBuffer[i].vdx = 0;
        oam->matrixBuffer[i].vdy = 1 << 8;
    }
    updateOAM(oam);
}
void initOAMTableSub(OAMTable * oam) {
    /*
     * For all 128 sprites on the DS, disable and clear any attributes they 
     * might have. This prevents any garbage from being displayed and gives 
     * us a clean slate to work with.
     */
    for (int i = 0; i < SPRITE_COUNT; i++) {
        oam->oamBuffer[i].attribute[0] = ATTR0_DISABLED;
        oam->oamBuffer[i].attribute[1] = 0;
        oam->oamBuffer[i].attribute[2] = 0;
        oam->oamBuffer[i].isHidden = true;
    }
    for (int i = 0; i < MATRIX_COUNT; i++) {
        /* If you look carefully, you'll see this is that affine trasformation
         * matrix again. We initialize it to the identity matrix, as we did
         * with backgrounds
         */
        oam->matrixBuffer[i].hdx = 1 << 8;
        oam->matrixBuffer[i].hdy = 0;
        oam->matrixBuffer[i].vdx = 0;
        oam->matrixBuffer[i].vdy = 1 << 8;
    }
    updateOAMSub(oam);
}
void rotateSprite(SpriteRotation * spriteRotation, int angle) {
    s16 s = sinLerp(angle) >> 4;
    s16 c = cosLerp(angle) >> 4;
    
    spriteRotation->hdx = c;
    spriteRotation->hdy = s;
    spriteRotation->vdx = -s;
    spriteRotation->vdy = c;
}
void setSpriteVisibility(SpriteEntry * spriteEntry, bool hidden, bool affine, bool doubleBound) {
    if (hidden) {
        /*
         * Make the sprite invisible.
         * 
         * An affine sprite cannot be hidden. We have to turn it into a
         * non-affine sprite before we can hide it. To hide any sprite, we must
         * set bit 8 and clear bit 9. For non-affine sprites, this is a bit
         * redundant, but it is faster than a branch to just set it regardless
         * of whether or not it is already set.
         */
        spriteEntry->isRotateScale = false; // Bit 9 off
        spriteEntry->isHidden = true; // Bit 8 on
    } else {
        /* Make the sprite visible.*/
        if (affine) {
            /* Again, keep in mind that affine sprites cannot be hidden, so
             * enabling affine is enough to show the sprite again. We also need
             * to allow the user to get the double bound flag in the sprite
             * attribute. If we did not, then our sprite hiding function would
             * not be able to properly hide and restore double bound sprites.
             * We enable bit 9 here because we want an affine sprite.
             */
            spriteEntry->isRotateScale = true;

            /* The double bound flag only acts as the double bound flag when
             * the sprite is an affine sprite. At all other times, it acts as
             * the sprite invisibility flag. We only enable bit 8 here if we want
             * a double bound sprite. */
            spriteEntry->isSizeDouble = doubleBound;
        } else {
            /* Bit 9 (the affine flag) will already be off here, so we don't
             * need to clear it. However, bit 8 (the sprite invisibility flag)
             * will need to be cleared. */
            spriteEntry->isHidden = false;
        }
    }
}
inline void setSpritePosition(SpriteEntry* spriteEntry,u16 x,u16 y){
    spriteEntry->x = x;
    spriteEntry->y = y;
}
inline void setSpritePriority(SpriteEntry* spriteEntry,ObjPriority priority){
    spriteEntry->priority = priority;
}

void initTypes(OAMTable * oam, SpriteInfo *spriteInfo,Type T1, Type T2) {
    
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                           * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
                                         sizeof(SPRITE_GFX_SUB[0]);

    /* Keep track of the available tiles */
    int nextAvailableTileIdx = 0;

    static const int TYPE_1_ID = 0;
    SpriteInfo * type1Info = &spriteInfo[TYPE_1_ID];
    SpriteEntry * type1 = &oam->oamBuffer[TYPE_1_ID];
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
    type1->x = SCREEN_WIDTH - 5 - type1Info->width - (T1 != T2 ? (2+type1Info->width):0);
    type1->size = OBJSIZE_32;
    type1->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += KampfTilesLen / BYTES_PER_16_COLOR_TILE;
    type1->priority = OBJPRIORITY_0;
    type1->palette = type1Info->oamId;
    
    static const int TYPE_2_ID = 1;
    SpriteInfo * type2Info = &spriteInfo[TYPE_2_ID];
    SpriteEntry * type2 = &oam->oamBuffer[TYPE_2_ID];

    if(T1 != T2) 
    {
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

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     TypePals[T1],
                     &SPRITE_PALETTE_SUB[type1Info->oamId * COLORS_PER_PALETTE],
                     COLORS_PER_PALETTE*2);
    if(T1 != T2)
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     TypePals[T2],
                     &SPRITE_PALETTE_SUB[type2Info->oamId * COLORS_PER_PALETTE],
                     COLORS_PER_PALETTE*2);
    /* Copy the sprite graphics to sprite graphics memory */
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     TypeTiles[T1],
                     &SPRITE_GFX_SUB[type1->gfxIndex * OFFSET_MULTIPLIER],
                     KampfTilesLen);
    if(T1 != T2)
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     TypeTiles[T2],
                     &SPRITE_GFX_SUB[type2->gfxIndex * OFFSET_MULTIPLIER],
                     KampfTilesLen);
}