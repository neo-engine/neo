/*
Pokémon neo
------------------------------

file        : mapSprite.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#pragma once
#include "mapDefines.h"
#include "sprite.h"

namespace MAP {
    constexpr u8 PLAYER_FAST = 20;

    struct mapSpriteInfo {
        u16 m_picNum;
        u8  m_curFrame;
        u8  m_width;
        u8  m_height;
    };

    struct mapSpriteData {
        u16 m_palData[ 16 ];
        u32 m_frameData[ 32 * 32 * 12 / 8 ]; // Down stop, down f1, down f2, up, up, up, left, l, l
    };

    class mapSprite {
      private:
        mapSpriteInfo _info;
        mapSpriteData _data;

      public:
        mapSprite( ) {
        }
        mapSprite( u16 p_currX, u16 p_currY, u16 p_imageId, u8 p_startFrame, u8 p_oamIdx,
                   u16 p_tileIdx );

        ObjPriority getPriority( );

        void setVisibility( bool p_value );
        void setPriority( ObjPriority p_value );

        void drawFrame( u8 p_value );
        void drawFrame( u8 p_value, bool p_hFlip );
        void setFrame( u8 p_value );

        void currentFrame( );
        void nextFrame( );
        void move( direction p_direction, s16 p_amount, bool p_update = true );
    };

    class mapSpriteManager {
        static constexpr u8 MAX_SMALL_NPC = 16;
        static constexpr u8 MAX_LARGE_NPC = 4;
        static constexpr u8 MAX_HM_PARTICLE = 32;
      private:
        mapSprite _player;         // 32x32
        mapSprite _playerPlatform; // 32x32

        std::pair<bool,mapSprite> _smallNpcs[ MAX_SMALL_NPC ]; // 16x32 (
        std::pair<bool,mapSprite> _bigNpcs[ MAX_LARGE_NPC ];    // 32x32

        mapSpriteData _strengthData;   // 16x16
        mapSpriteData _rockSmashData;  // 16x16
        mapSpriteData _rockSmashData2; // crushed rock (16x16)
        mapSpriteData _cutData;        // 16x16
        mapSpriteData _cutData2;       // cut tree (16x16)
        mapSpriteData _rustlingGrass;
        mapSpriteData _rustlingLargeGrass;
        mapSpriteData _waterBubbles;

        std::pair<u8,mapSpriteInfo> _hmSpriteInfo[ MAX_HM_PARTICLE ];

      public:
        void init( );

        /*
         * @brief: Removes all sprites that are more than p_radius blocks away from
         * (p_centerX, p_centerY) (in max norm)
         */
        void purge( u16 p_centerX, u16 p_centerY, u16 p_radius );

        /*
         * @brief: Moves the camera in the specified direction. (moves all sprites by the
         * specified amount)
         */
        void moveCamera( u8 p_spriteId, direction p_direction, s16 p_amount, bool p_movePlayer = false );

        /*
         * @brief: Loads an NPC sprite at the given position, returns an id for the sprite
         * that needs to be passed when the sprite should be accessed.
         */
        u8 loadNPCSprite( u16 p_currX, u16 p_currY, u16 p_imageId, u8 p_startFrame );

        u8 loadHMParticle( u16 p_currX, u16 p_currY, u16 p_imageId );

        u8 loadPlayerSprite( u16 p_currX, u16 p_currY, u16 p_imageId, u8 p_startFrame );

        u8 loadPlayerPlatform( u16 p_currX, u16 p_currY, u16 p_imageId, u8 p_startFrame );

        /*
         * @brief: Hides the specified sprite and marks its corresponding slot as free to
         * use.
         */
        void destroySprite( u8 p_spriteId );

        /*
         * @brief: Moves the specified sprite by the specified amount in the specified
         * direction.
         */
        void moveSprite( u8 p_spriteId, direction p_direction, s16 p_amount, bool p_update = true );

        /*
         * @brief: Returns the current priority of the specified sprite.
         */
        ObjPriority getPriority( u8 p_spriteId ) const;

        /*
         * @brief: Sets the visibility of the specified sprite.
         */
        void setVisibility( u8 p_spriteId, bool p_value );

        /*
         * @brief: Sets the priority of the specified sprite.
         */
        void setPriority( u8 p_spriteId, ObjPriority p_value );

        /*
         * @brief: Draws the specified frame of the specified sprite.
         */
        void drawFrame( u8 p_spriteId, u8 p_value );

        /*
         * @brief: Draws the specified frame of the specified sprite.
         */
        void drawFrame( u8 p_spriteId, u8 p_value, bool p_hFlip );

        /*
         * @brief: Sets the specified frame of the specified sprite.
         */
        void setFrame( u8 p_spriteId, u8 p_value );

        void currentFrame( );
        void nextFrame( );
    };
} // namespace MAP
