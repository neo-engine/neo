/*
Pokémon neo
------------------------------

file        : statusScreenUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <vector>

#include "move.h"
#include "pokemon.h"
#include "pokemonData.h"
#include "uio.h"

namespace STS {
    class statusScreenUI {
      public:
        static constexpr u8 BACK_TARGET = 252;
        static constexpr u8 EXIT_TARGET = 251;
        static constexpr u8 NEXT_TARGET = 250;
        static constexpr u8 PREV_TARGET = 249;

      private:
        u8             _currentPage = 255;
        bool           _allowKeyUp;
        bool           _allowKeyDown;
        bool           _needsInit;
        pkmnData       _data;
        MOVE::moveData _moves[ 4 ];

        u16 initTopScreen( pokemon* p_pokemon, bool p_bottom = false );
        u16 initBottomScreen( pokemon* p_pokemon, bool p_bottom = true );

        /*
         * @brief: Draws basic information about the pkmn (name, etc)
         */
        void drawBasicInfoTop( pokemon* p_pokemon, bool p_bottom = false );

        /*
         * @brief: Writes the given string to the specified line to the text field.
         */
        void writeLineTop( const char* p_string, u8 p_line, u8 p_color = 252,
                           bool p_bottom = false );
        /*
         * @brief: Writes the given string to the specified line to the text field.
         */
        void writeLineTop( const char* p_stringLeft, const char* p_stringRight, u8 p_line,
                           u8 p_colorLeft = 250, u8 p_colorRight = 252, bool p_bottom = false );

      public:
        /*
         * @brief: Initializes the status screen UI. Destroys anything that was previously on the
         * screen.
         * @param p_initialPage: initially selected page
         */
        void init( pokemon* p_pokemon, u8 p_initialPage, bool p_allowKeyUp = true,
                   bool p_allowKeyDown = true );

        /*
         * @brief: Returns the number of different pages.
         */
        constexpr u8 getPageCount( ) {
            // TODO: Add Contest Screen
            return 2;
        }

        /*
         * @brief: Returns the different choices for the given page.
         */
        constexpr u8 getDetailsPageCount( pokemon* p_pokemon, u8 p_page ) {
            if( p_pokemon->isEgg( ) ) { return 0; }
            if( p_page == 1 ) {
                u8 numMoves = 0;
                for( ; numMoves < 4; ++numMoves ) {
                    if( !p_pokemon->getMove( numMoves ) ) { break; }
                }
                return numMoves;
            }
            return 0;
        }

        /*
         * @brief: Highlights the specified button.
         */
        void highlightButton( u8 p_button = 255, bool p_bottom = true );

        /*
         * @brief: Draws the specified info page for the given pokemon.
         */
        void draw( pokemon* p_pokemon, u8 p_page );

        /*
         * @brief: Draws the specified details (i.e. move details, etc) for the given page.
         */
        void showDetails( pokemon* p_pokemon, u8 p_page, u8 p_detailsPage );

        /*
         * @brief: Draws the p_frame-th frame. Needs to be called every frame.
         */
        void animate( u8 p_frame );

        /*
         * @brief: Returns information for the current buttons on the screen.
         */
        std::vector<std::pair<IO::inputTarget, u8>> getTouchPositions( bool p_bottom = true );
    };
} // namespace STS
