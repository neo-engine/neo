/*
Pokémon neo
------------------------------

file        : navApp.h
author      : Philip Wellnitz
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <vector>
#include "defines.h"
#include "uio.h"

namespace NAV {
    class navApp {
      public:
        /*
         * @brief: Draws the icon of the application at the specified position on the
         * screen.
         */
        virtual void drawIcon( u8 p_oamSlot, bool p_bottom = true ) = 0;

        /*
         * @brief: Loads the application.
         */
        virtual void load( bool p_bottom = true ) = 0;

        /*
         * @brief: Updates the application and handles input.
         * @returns: true if the app exited.
         */
        virtual bool tick( bool p_bottom = true ) = 0;
    };

    class mapNavApp : public navApp {
        static constexpr u16 MAP_TOP_X = 4;
        static constexpr u16 MAP_TOP_Y = 36;
        static constexpr u16 MAP_BOT_X = 219;
        static constexpr u16 MAP_BOT_Y = 155;

        static constexpr u16 MAP_IMG_RES = 4;

        u16 _cursorX, _cursorY;
        u16 _cursorLocationId;
        u16 _playerX, _playerY;

        void computePlayerPosition( );
        void computeCursorLocationId( );

      public:
        inline mapNavApp( ) {
            computePlayerPosition( );
            _cursorX = _playerX;
            _cursorY = _playerY;
            computeCursorLocationId( );
        }

        void drawIcon( u8 p_oamSlot, bool p_bottom = true );
        void load( bool p_bottom = true );
        bool tick( bool p_bottom = true );
    };

    class jboxNavApp : public navApp {
        static constexpr u16 SONGS_PER_PAGE  = 5;
        static constexpr u16 EXIT_CHOICE     = 0;
        static constexpr u16 FWD_CHOICE      = 1;
        static constexpr u16 BWD_CHOICE      = 2;
        static constexpr u16 STOP_PLAYBACK   = 3;
        static constexpr u16 NUM_SPECIAL_TGS = 4;

        u16 _currentSelStart;
        u16 _currentSong;

        void drawSongList( u16 p_startIdx, bool p_bottom = true );
        void selectSong( u16 p_idx, bool p_bottom = true );
        void hoverButton( u16 p_btn, bool p_bottom = true );
        std::vector<std::pair<IO::inputTarget, u16>> touchPositions( bool p_bottom = true );

      public:
        inline jboxNavApp( ) {
            _currentSelStart = 1;
            _currentSong     = 0;
        }

        void drawIcon( u8 p_oamSlot, bool p_bottom = true );
        void load( bool p_bottom = true );
        bool tick( bool p_bottom = true );
    };

} // namespace NAV
