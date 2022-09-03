/*
 Pokémon neo
 ------------------------------

 file : sprite.h
 author : Philip Wellnitz
 description : Header file. Consult the corresponding source file for details.

 Copyright (C) 2012 - 2022
 Philip Wellnitz

 This file is part of Pokémon neo.

 Pokémon neo is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Pokémon neo is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Pokémon neo. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <nds.h>

#include "battle/move.h"
#include "battle/type.h"
#include "save/saveOptions.h"

namespace IO {
    extern const char* OW_PATH;
    extern const char* DOOR_PATH;
    extern const char* OWP_PATH;
    extern const char* TRAINER_PATH;
    extern const char* BERRY_PATH;

    struct SpriteInfo {
        u8           m_oamId;
        u8           m_width;
        u8           m_height;
        u16          m_angle;
        SpriteEntry* m_entry;
    };

    void updateOAM( bool p_bottom );

    void initOAMTable( bool p_bottom );

    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, bool p_bottom );
    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, const u16 p_palLen,
                        bool p_bottom );
    void copySpritePal( const unsigned short* p_spritePal, const u8 p_palIdx, const u8 p_startIdx,
                        const u16 p_palLen, bool p_bottom );
    void copySpriteData( const unsigned int* p_spriteData, const u16 p_tileIdx,
                         const u32 p_spriteDataLen, bool p_bottom );

    /*
     * @brief: Loads the given tiles and pal to the specified position in the OAM(Sub)
     * (Assumes 1D tiled sprites w/ 16 colors per palette)
     */
    u16 loadSprite( const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileIdx, const s16 p_posX,
                    const s16 p_posY, const u8 p_width, const u8 p_height,
                    const unsigned short* p_spritePal, const unsigned int* p_spriteData,
                    const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                    ObjPriority p_priority, bool p_bottom,
                    ObjBlendMode p_blendMode = OBJMODE_NORMAL );

    /*
     * @brief: Loads the given tiles and pal to the specified position in the OAM(Sub)
     * (Assumes 1D tiled sprites w/ 16 colors per palette)
     */
    u16 loadSprite( const char* p_name, const u8 p_oamIdx, const u8 p_palIdx, const u16 p_tileIdx,
                    const s16 p_posX, const s16 p_posY, const u8 p_width, const u8 p_height,
                    bool p_flipX, bool p_flipY, bool p_hidden, ObjPriority p_priority,
                    bool p_bottom, ObjBlendMode p_blendMode = OBJMODE_NORMAL );

    /*
     * @brief: Loads the given tiles and pals to the OAM(Sub); assumes bitmap mode.
     * @param p_tiled: Specifies whether the given p_spriteData is tiled (default: true)
     */
    u16 loadSpriteB( const u8 p_oamIdx, const u16 p_tileIdx, const s16 p_posX, const s16 p_posY,
                     const u8 p_width, const u8 p_height, const unsigned short* p_spritePal,
                     const unsigned int* p_spriteData, const u32 p_spriteDataLen, bool p_flipX,
                     bool p_flipY, bool p_hidden, ObjPriority p_priority, bool p_bottom,
                     bool p_outline = false, u16 p_outlineColor = 0xFFFF, bool p_tiled = true,
                     u8 p_transparency = 15 );
    /*
     * @brief: Loads the sprite data to the OAM(Sub); assumes bitmap mode.
     */
    u16 loadSpriteB( const u8 p_oamIdx, const u16 p_tileIdx, const s16 p_posX, const s16 p_posY,
                     const u8 p_width, const u8 p_height, const unsigned short* p_spriteData,
                     const u32 p_spriteDataLen, bool p_flipX, bool p_flipY, bool p_hidden,
                     ObjPriority p_priority, bool p_bottom, u8 p_transparency = 15 );

    /*
     * @brief: Loads the sprite with the given name from the FS. Assumes bitmap mode.
     */
    u16 loadSpriteB( const char* p_name, const u8 p_oamIdx, const u16 p_tileIdx, const s16 p_posX,
                     const s16 p_posY, const u8 p_width, const u8 p_height, bool p_flipX,
                     bool p_flipY, bool p_hidden, ObjPriority p_priority, bool p_bottom,
                     bool p_outline = false, u16 p_outlineColor = 0xFFFF, bool p_tiled = true,
                     u8 p_transparency = 15 );

    /*
     * @brief: Returns the number of empty pixels below the pkmn in its sprite.
     */
    u16 pkmnSpriteHeight( const pkmnSpriteInfo& p_pkmn );

    u16 loadPKMNSprite( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                        u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom,
                        bool p_blackOverlay = false );

    u16 loadPKMNSpriteB( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                         u8 p_oamIndex, u16 p_tileCnt, bool p_bottom, bool p_blackOverlay = false );

    /*
     * @brief: Loads the back sprite for the given pkmn.
     */
    u16 loadPKMNSpriteBack( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                            u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom,
                            bool p_blackOverlay = false );

    /*
     * @brief: Loads the specified pokemon icon from the nitro FAT. (1D tiled)
     */
    u16 loadPKMNIcon( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                      u8 p_oamIndex, u8 p_palcnt, u16 p_tileCnt, bool p_bottom = true,
                      bool p_blackOverlay = false );
    /*
     * @brief: Loads the specified pokemon icon from the nitro FAT. (1D bitmap)
     */
    u16 loadPKMNIconB( const pkmnSpriteInfo& p_pkmn, const s16 p_posX, const s16 p_posY,
                       u8 p_oamIndex, u16 p_tileCnt, bool p_bottom = true, bool p_outline = false,
                       u16 p_outlineColor = 0xFFFF, bool p_blackOverlay = false );

    /*
     * @brief: Loads an egg icon from the nitro FAT. (1D tiled)
     */
    u16 loadEggIcon( const s16 p_posX, const s16 p_posY, u8 p_oamIndex, u8 p_palcnt, u16 p_tileCnt,
                     bool p_bottom = true, bool p_manaphy = false );
    /*
     * @brief: Loads an egg icon from the nitro FAT. (1D bitmap)
     */
    u16 loadEggIconB( const s16 p_posX, const s16 p_posY, u8 p_oamIndex, u16 p_tileCnt,
                      bool p_bottom = true, bool p_manaphy = false, bool p_outline = false,
                      u16 p_outlineColor = 0xFFFF );

    u16 loadEggSprite( const s16 p_posX, const s16 p_posY, u8 p_oamIndex, u8 p_palCnt,
                       u16 p_tileCnt, bool p_bottom = false, bool p_manaphy = false );

    u16 loadTrainerSprite( u8 p_trainerId, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                           u8 p_palCnt, u16 p_tileCnt, bool p_bottom );

    u16 loadAnimatedSprite( FILE* p_file, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                            u8 p_palCnt, u16 p_tileCnt, ObjPriority p_priority = OBJPRIORITY_2,
                            bool p_bottom = false );
    u16 loadAnimatedSpriteB( FILE* p_file, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                             u16 p_tileCnt, ObjPriority p_priority = OBJPRIORITY_2,
                             bool p_bottom = false, bool p_outline = false,
                             u16 p_outlineColor = 0xFFFF, bool p_blackOverlay = false );

    u16 loadOWSprite( const u16 p_picnum, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                      u8 p_palCnt, u16 p_tileCnt );
    u16 loadOWSpriteB( const u16 p_picnum, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                       u16 p_tileCnt, u16 p_palData[ 16 ], u32 p_data[ 32 * 4 * 9 ] );

    u16 loadDoorSpriteB( const u16 p_doorNum, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                         u16 p_tileCnt, u16 p_palData[ 16 ], u32 p_data[ 32 * 4 * 9 ] );

    void setOWSpriteFrame( u8 p_frame, bool p_flip, u8 p_oamIndex, u16 p_palData[ 16 ],
                           u32 p_data[ 32 * 4 * 9 ], bool p_vflip = false );

    void setAnimatedSpriteFrame( u8 p_frame, bool p_hFlip, u8 p_oamIndex, u16 p_tileCnt );

    /*
     * @brief: Loads the specified icon from the nitro FAT. (1D tiled)
     */
    u16 loadIcon( const char* p_path, const char* p_name, const s16 p_posX, const s16 p_posY,
                  u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
    /*
     * @brief: Loads the specified icon from the nitro FAT. (1D bitmap)
     */
    u16 loadIconB( const char* p_path, const char* p_name, const s16 p_posX, const s16 p_posY,
                   u8 p_oamIndex, u16 p_tileCnt, bool p_bottom );

    u16 loadItemIcon( const u16 p_itemId, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                      u8 p_palcnt, u16 p_tileCnt, bool p_bottom = true );
    u16 loadItemIconB( const u16 p_itemId, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                       u16 p_tileCnt, bool p_bottom = true );

    u16 loadTMIcon( BATTLE::type p_type, u8 p_tmtype, const s16 p_posX, const s16 p_posY,
                    u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom = true );
    u16 loadTMIconB( BATTLE::type p_type, u8 p_tmtype, const s16 p_posX, const s16 p_posY,
                     u8 p_oamIndex, u16 p_tileCnt, bool p_bottom = true );

    u16 loadTypeIcon( BATTLE::type p_type, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                      u8 p_palCnt, u16 p_tileCnt, bool p_bottom, SAVE::language p_language );
    u16 loadTypeIcon( BATTLE::type p_type, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                      u16 p_tileCnt, bool p_bottom, SAVE::language p_language );

    u16 loadContestTypeIcon( BATTLE::contestType p_type, const s16 p_posX, const s16 p_posY,
                             u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom,
                             SAVE::language p_language );
    u16 loadContestTypeIcon( BATTLE::contestType p_type, const s16 p_posX, const s16 p_posY,
                             u8 p_oamIndex, u16 p_tileCnt, bool p_bottom,
                             SAVE::language p_language );

    u16 loadDamageCategoryIcon( BATTLE::moveHitTypes p_type, const s16 p_posX, const s16 p_posY,
                                u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
    u16 loadDamageCategoryIcon( BATTLE::moveHitTypes p_type, const s16 p_posX, const s16 p_posY,
                                u8 p_oamIndex, u16 p_tileCnt, bool p_bottom );

    u16 loadLocationBackB( u8 p_idx, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                           u16 p_tileCnt, bool p_bottom );
    u16 loadPlatform( u8 p_platformIdx, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                      u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
    u16 loadRibbonIcon( u8 p_ribbonIdx, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                        u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
    u16 loadShapeIcon( u8 p_shapeIdx, const s16 p_posX, const s16 p_posY, u8 p_oamIndex,
                       u8 p_palCnt, u16 p_tileCnt, bool p_bottom );
} // namespace IO
