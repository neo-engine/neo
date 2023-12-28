/*
Pokémon neo
------------------------------

file        : mapSprite.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2023
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

#pragma once
#include "io/sprite.h"
#include "map/mapDefines.h"

namespace MAP {
    constexpr u8 PLAYER_FAST = 20;

    constexpr u16 PKMN_SPRITE = 1000;
    struct mapSpriteInfo {
        u16 m_picNum;
        u8  m_curFrame;
    };

    struct mapSpritePos {
        u16 m_curX; // global x coordinate of current position in OW
        u16 m_curY; // global y coordinate of current position in OW
        s8  m_dx;   // shift in x dir (fractional part of the ow position)
        s8  m_dy;   // shift in x dir (fractional part of the ow position)

        s16 m_camDisX; // signed x distance to camera position (used to determine when to show/hide
                       // the sprite)
        s16 m_camDisY; // signed y distance to camera position

        /*
         * @brief: Shifts the sprite's position relative to the camera, but does not change
         * its OW position.
         */
        constexpr void translateSprite( s8 p_dx, s8 p_dy ) {
            m_camDisX += p_dx;
            m_camDisY += p_dy;
        }

        /*
         * @brief: Moves the sprite to a new position in the OW.
         */
        constexpr void moveSprite( s8 p_dx, s8 p_dy ) {
            translateSprite( p_dx, p_dy );

            m_dx += p_dx;
            if( m_dx > 16 || m_dx < -16 ) {
                m_curX += m_dx / 16;
                m_dx = m_dx % 16;
            }
            m_dy += p_dy;
            if( m_dy > 16 || m_dy < -16 ) {
                m_curY += m_dy / 16;
                m_dy = m_dy % 16;
            }
        }

        /*
         * @brief: Returns whether a sprite is visible if it is located at this position.
         */
        constexpr bool isVisible( ) const {
            if( m_camDisY < -120 || m_camDisY > 120 ) { return false; }
            if( m_camDisX < -160 || m_camDisX > 160 ) { return false; }
            return true;
        }
    };

    struct mapSpriteData {
        u8  m_width;
        u8  m_height;
        u8  m_frameCount;
        u16 m_palData[ 16 ];
        u32 m_frameData[ 32 * 32 * 12 / 8 ]; // Down stop, down f1, down f2, up, up, up, left, l, l

        constexpr mapSpriteData( ) {
        }

        /*
         * @brief: reads the sprite data from the FS; if p_imageId < 250, it is
         * interpreted as a player sprite; if p_imageId == 250, loads the current player
         * appearance's sprite; p_imageId == 251 loads the rival's sprite.
         * Anything larger than PKMN_SPRITE loads a pkmn ow sprite.
         */
        mapSpriteData( u16 p_imageId, u8 p_forme = 0, bool p_shiny = false, bool p_female = false );

        /*
         * @brief: reads the door data from the fs.
         */
        mapSpriteData( u8 p_door, u16 p_palData[ 16 ] );

        /*
         * @brief: Read mapSpriteData from the specified file.
         */
        void readData( FILE* p_f, bool p_close = true );

        /*
         * @brief: Copies BG_PALETE[ 16 * p_bgPalIdx, .., 16 * p_bgPalIdx + 15 ] to this
         * sprites palette data.
         */
        void updatePalette( u8 p_bgPalIdx );
    };

    class mapSprite {
      private:
        mapSpriteInfo _info;
        mapSpriteData _data;

      public:
        mapSprite( ) {
        }
        mapSprite( u16 p_imageId, u8 p_startFrame, u8 p_forme = 0, bool p_shiny = false,
                   bool p_female = false );

        mapSprite( FILE* p_f, u8 p_startFrame );

        mapSprite( mapSpriteInfo p_info, const mapSpriteData& p_data )
            : _info( p_info ), _data( p_data ) {
        }

        const mapSpriteData& getData( ) const {
            return _data;
        }

        mapSpriteData& getData( ) {
            return _data;
        }

        /*
         * @brief: Normalizes p_value and draws the resulting frame.
         */
        void drawFrame( u8 p_oamIdx, u8 p_value );

        /*
         * @brief: Draws the first frame of the given direction.
         */
        void drawFrameD( u8 p_oamIdx, direction p_direction );

        /*
         * @brief: Draws the specified frame, doesn't change the internal frame of the
         * sprite.
         */
        void drawFrame( u8 p_oamIdx, u8 p_value, bool p_hFlip );

        /*
         * @brief: Sets the sprite to the specified frame and draws that frame.
         */
        void setFrame( u8 p_oamIdx, u8 p_value );

        void setFrameD( u8 p_oamIdx, direction p_direction );

        /*
         * @brief: Draws the current frame of the sprite.
         */
        void currentFrame( u8 p_oamIdx );

        /*
         * @brief: Increments the frame and redraws the sprite.
         */
        void nextFrame( u8 p_oamIdx );

        constexpr u8 getCurrentFrame( ) const {
            return _info.m_curFrame;
        }

        u8 getFrameForDir( direction p_direction ) const;
    };

    class mapSpriteManager {
      public:
        static constexpr u8 MAX_SMALL_NPC             = 28;
        static constexpr u8 MAX_LARGE_NPC             = 5;
        static constexpr u8 MAX_EXTRA_LARGE_NPC       = 1;
        static constexpr u8 MAX_HM_PARTICLE           = 16;
        static constexpr u8 MAX_HM_PARTICLE_GFX_SLOTS = 6;
        static constexpr u8 MAX_TILE_ANIM             = 16;
        static constexpr u8 MAX_TILE_ANIM_GFX_SLOTS   = 10;

        static constexpr u8 SPR_UNUSED    = 0;
        static constexpr u8 SPR_ITEM      = 1;
        static constexpr u8 SPR_HMBALL    = 2;
        static constexpr u8 SPR_STRENGTH  = 3;
        static constexpr u8 SPR_ROCKSMASH = 4;
        static constexpr u8 SPR_CUT       = 5;
        static constexpr u8 SPR_PLATFORM  = 8;

        static constexpr u8 SPR_DOOR = 99;

        enum tileAnimation : u8 {
            TILE_ANIM_START            = 100,
            SPR_GRASS                  = 100,
            SPR_LONG_GRASS             = 101,
            SPR_GRASS_SHINY            = 102,
            SPR_FOOTPRINT              = 103,
            SPR_FOOTPRINT_VERTICAL     = 103,
            SPR_FOOTPRINT_HORIZONTAL   = 104,
            SPR_WATER_CIRCLE           = 105,
            SPR_DIVE_BUBBLE            = 106,
            SPR_FOOTPRINT_BIKE         = 107,
            SPR_FOOTPRINT_BIKE_FRAME_1 = 107,
            SPR_FOOTPRINT_BIKE_FRAME_4 = 108, // shares gfx slot w/ f1
            SPR_FOOTPRINT_BIKE_FRAME_2 = 109,
            SPR_FOOTPRINT_BIKE_FRAME_3 = 110, // shares gfx slot w/ f3
            SPR_HOT_SPRING_WATER       = 111,
        };

        static constexpr u8 SPR_MAPTILE_GFX_SLOT_1 = 0;
        static constexpr u8 SPR_MAPTILE_GFX_SLOT_2 = 2;
        static constexpr u8 SPR_MAPTILE_GFX_SLOT_3 = 4;
        static constexpr u8 SPR_MAPTILE_GFX_SLOT_4 = 6;

        enum spriteType {
            SPTYPE_NONE       = 0,
            SPTYPE_PLAYER     = 1,
            SPTYPE_PLATFORM   = 2,
            SPTYPE_NPC        = 3, // Trainer, berry trees
            SPTYPE_PARTICLE   = 4, // item icon, hm particles, etc
            SPTYPE_BERRYTREE  = 5,
            SPTYPE_TIELSETAUX = 6, // grass animation, etc
            SPTYPE_DOOR       = 7,
        };

        struct managedSprite {
            mapSprite    m_sprite;
            mapSpritePos m_pos;
            spriteType   m_type;
            bool         m_reflectionVisible;

            constexpr void translateSprite( s8 p_dx, s8 p_dy ) {
                m_pos.translateSprite( p_dx, p_dy );
            }

            constexpr void moveSprite( s8 p_dx, s8 p_dy ) {
                m_pos.moveSprite( p_dx, p_dy );
            }

            constexpr bool isVisible( ) const {
                return m_pos.isVisible( );
            }
        };

        static u8 animationExpiry( u8 p_animation ) {
            switch( p_animation ) {
            case SPR_FOOTPRINT_HORIZONTAL:
            case SPR_FOOTPRINT_VERTICAL: return 30;
            case SPR_FOOTPRINT_BIKE_FRAME_1:
            case SPR_FOOTPRINT_BIKE_FRAME_2:
            case SPR_FOOTPRINT_BIKE_FRAME_3:
            case SPR_FOOTPRINT_BIKE_FRAME_4: return 20;
            case SPR_WATER_CIRCLE: return 5;
            case SPR_DIVE_BUBBLE: return 4;
            default: return 255; // never
            }
        }
        static u8 animationNextFrame( u8 p_animation, u8 p_currentFrame ) {
            switch( p_animation ) {
            case SPR_FOOTPRINT_HORIZONTAL:
            case SPR_FOOTPRINT_VERTICAL: return 0;
            case SPR_FOOTPRINT_BIKE_FRAME_1:
            case SPR_FOOTPRINT_BIKE_FRAME_2:
            case SPR_FOOTPRINT_BIKE_FRAME_3:
            case SPR_FOOTPRINT_BIKE_FRAME_4: return 0;
            case SPR_WATER_CIRCLE: return ( p_currentFrame + 1 ) % 5;
            case SPR_DIVE_BUBBLE: return ( p_currentFrame + 1 ) % 8;
            default: return 0; // never
            }
        }

      private:
        managedSprite _player;         // 32x32
        managedSprite _playerPlatform; // 32x32

        std::pair<bool, managedSprite> _smallNpcs[ MAX_SMALL_NPC ]; // 16x32
        std::pair<bool, managedSprite> _bigNpcs[ MAX_LARGE_NPC ];   // 32x32 (or 1x 32x32
                                                                    // and 1x 64x64)

        mapSpriteData _itemBallData;       // 16x16
        mapSpriteData _hmBallData;         // 16x16
        mapSpriteData _strengthData;       // 16x16
        mapSpriteData _rockSmashData;      // 16x16
        mapSpriteData _cutData;            // 16x16
        mapSpriteData _grassData;          // 16x16
        mapSpriteData _shinyGrassData;     // 16x16
        mapSpriteData _longGrassData;      // 16x16
        mapSpriteData _footprintData;      // 16x16
        mapSpriteData _footprintBikeData;  // 16x16
        mapSpriteData _waterCircleData;    // 16x16
        mapSpriteData _diveBubbleData;     // 32x16
        mapSpriteData _hotSpringWaterData; // 16x16

        managedSprite _doorAnimation; // 32x16

        // mapSpriteData _waterBubbles;

        u8 _oamPosition[ 128 ];  // positions of the sprites in the oam
        u8 _oamPositionR[ 128 ]; // positions of the sprites in the oam

        std::pair<u8, mapSpritePos> _hmSpriteInfo[ MAX_HM_PARTICLE ];
        std::pair<u8, mapSpritePos> _tileAnimInfo[ MAX_TILE_ANIM ];

        bool _hasExtraLargeSprite = false;

        constexpr s16 camShift( u16 p_cam, u16 p_pos ) const {
            return ( p_pos - p_cam ) * 16;
        }

        constexpr u16 screenX( u16 p_camX, u16 p_posX, u8 p_width = 16 ) const {
            return 128 - p_width / 2 + camShift( p_camX, p_posX );
        }

        constexpr u16 screenY( u16 p_camY, u16 p_posY, u8 p_height = 32 ) const {
            return 96 - ( p_height - 8 ) + camShift( p_camY, p_posY );
        }

        managedSprite& getManagedSprite( u8 p_spriteId );

        const managedSprite& getManagedSprite( u8 p_spriteId ) const;

        /*
         * @brief: Returns the mapSprite corresponding to a sprite id.
         */
        mapSpriteData& getSpriteData( u8 p_spriteId );

        const mapSpriteData& getSpriteData( u8 p_spriteId ) const;

        /*
         * @brief: Swaps two sprites in the OAM.
         */
        void swapSprites( u8 p_spriteId1, u8 p_spriteId2, bool p_update = true );

      public:
        /*
         * @brief: Rearranges sprites in OAM so that a sprite with higher y-coordinate
         * appears earlier in the OAM, making it hide the sprites with smaller
         * y-coordinate and same priority.
         */
        void reorderSprites( bool p_update = true );

        void reset( );

        void init( );

        /*
         * @brief: Moves the camera in the specified direction. (moves all sprites by the
         * specified amount)
         */
        void moveCamera( direction p_direction, s16 p_amount, bool p_movePlayer = false );

        /*
         * @brief: Loads the given sprite, returns an id for the sprite
         * that needs to be passed when the sprite should be accessed.
         * @param p_camX: x coordinate of the current camera position
         * @param p_camY: y coordinate of the current camera position
         */
        u8 loadSprite( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY, u8 p_posZ, spriteType p_type,
                       const mapSprite& p_sprite, bool p_hidden = false );

        /*
         * @brief: Loads a (possibly new) instance of the given sprite type, returns an id for the
         * sprite that needs to be passed when the sprite should be accessed.
         * @param p_camX: x coordinate of the current camera position
         * @param p_camY: y coordinate of the current camera position
         */
        u8 loadSprite( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY, u8 p_posZ, u8 p_partilcleId,
                       bool p_hidden = false );

        /*
         * @brief: Loads a berry tree and displays it at the specified stage.
         * @param p_camX: x coordinate of the current camera position
         * @param p_camY: y coordinate of the current camera position
         */
        u8 loadBerryTree( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY, u8 p_berryIdx,
                          u8 p_stage );

        /*
         * @brief: Loads the specified door animation to the specified position on the
         * screen.
         * @param p_palette: The pert of the OW palette as specified by DOOR_ANIMATIONS.
         */
        u8 loadDoor( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY, u8 p_doorIdx,
                     u16 p_palette[ 16 ] );

        /*
         * @brief: loads a sprite centered on the screen.
         * @param p_camX: x coordinate of the current camera position
         * @param p_camY: y coordinate of the current camera position
         */
        inline u8 loadSprite( u16 p_camX, u16 p_camY, spriteType p_type, const mapSprite& p_sprite,
                              bool p_hidden = false ) {
            return loadSprite( p_camX, p_camY, p_camX, p_camY, 3, p_type, p_sprite, p_hidden );
        }

        inline u8 loadSprite( u16 p_camX, u16 p_camY, u8 p_partilcleId, bool p_hidden = false ) {
            return loadSprite( p_camX, p_camY, p_camX, p_camY, 3, p_partilcleId, p_hidden );
        }

        /*
         * @brief: Hides the specified sprite and marks its corresponding slot as free to
         * use.
         */
        void destroySprite( u8 p_spriteId, bool p_update = true );

        /*
         * @brief: Moves the specified sprite by the specified amount in the specified
         * direction.
         */
        void moveSprite( u8 p_spriteId, direction p_direction, s16 p_amount, bool p_update = true );

        void moveSprite( u8 p_spriteId, u8 p_targetSpriteId, bool p_update = true );

        void moveSpriteT( u8 p_spriteId, u16 p_targetX, u16 p_targetY, bool p_update = true );

        void moveSprite( u8 p_spriteId, s8 p_dx, s8 p_dy, bool p_update = true );

        /*
         * @brief: Changes the position relative to the camera of the specified sprite.
         */
        void translateSprite( u8 p_spriteId, s8 p_dx, s8 p_dy, bool p_update = true );

        /*
         * @brief: Returns the current priority of the specified sprite.
         */
        ObjPriority getPriority( u8 p_spriteId ) const;

        /*
         * @brief: Sets the visibility of the specified sprite.
         */
        void setVisibility( u8 p_spriteId, bool p_value, bool p_update = true );

        bool getVisibility( u8 p_spriteId );

        /*
         * @brief: Sets the priority of the specified sprite.
         */
        void setPriority( u8 p_spriteId, ObjPriority p_value, bool p_update = true );

        /*
         * @brief: Draws the specified frame of the specified sprite.
         */
        void drawFrame( u8 p_spriteId, u8 p_value, bool p_update = true );

        void drawFrameD( u8 p_spriteId, direction p_direction, bool p_update = true );

        /*
         * @brief: Draws the specified frame of the specified sprite.
         */
        void drawFrame( u8 p_spriteId, u8 p_value, bool p_hFlip, bool p_update = true,
                        bool p_vFlip = false );

        /*
         * @brief: Sets the specified frame of the specified sprite.
         */
        void setFrame( u8 p_spriteId, u8 p_value, bool p_update = true );

        void setFrameD( u8 p_spriteId, direction p_direction, bool p_update = true );

        void currentFrame( u8 p_spriteId, bool p_update = true );
        void nextFrame( u8 p_spriteId, bool p_update = true );

        /*
         * @brief: Shows an emotion bubble above the specified sprite. (defaults to an
         * exclamation mark)
         */
        void showExclamation( u8 p_spriteId, u8 p_emote = 0 );

        void hideExclamation( );

        /*
         * @brief: Creates a y-flipped copy of the specified sprite that moves with the
         * corresponding non-mirrored sprite.
         */
        void enableReflection( u8 p_spriteId );

        void disableReflection( u8 p_spriteId );

        void update( );
    };
} // namespace MAP
