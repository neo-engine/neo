/*
Pokémon neo
------------------------------

file        : startScreen.h
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
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "save/saveGame.h"

namespace SAVE {
    class startScreen {
      public:
        typedef u8 choice;

      private:
        u8       _currentSlot = 0;
        language _currentLanguage;

        static constexpr choice CONTINUE        = 0;
        static constexpr choice NEW_GAME        = 1;
        static constexpr choice SPECIAL_EPISODE = 2;
        static constexpr choice TRANSFER_GAME   = 3;
        static constexpr choice MYSTERY_GIFT    = 4;

        constexpr u16 getTextForMainChoice( choice p_choice ) const {
            switch( p_choice ) {
            case CONTINUE: return 71;
            case NEW_GAME: return 72;
            case SPECIAL_EPISODE: return 73;
            case TRANSFER_GAME: return 74;
            case MYSTERY_GIFT: return 404;
            default: return 0;
            }
        }

        std::vector<choice> getMainChoicesForSlot( u8 p_slot );

        std::vector<std::pair<IO::inputTarget, choice>>
        drawMainChoice( const std::vector<choice>& p_choices, u8 p_slot );

        std::vector<std::pair<IO::inputTarget, choice>> drawEpisodeChoice( );

        void selectMainChoice( u8 p_choice );

        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
        printYNMessage( const char* p_message, u8 p_selection );

        /*
         * @brief: Makes the player select a special episode.
         */
        u8 runEpisodeChoice( );

        /*
         * @brief: Makes the player select a language.
         */
        language runLanguageChoice( );

        bool initNewGame( gameType p_type, language p_lang, u8 p_episode = 0 );

        /*
         * @brief: Converts the GBA save file into a playable copy.
         */
        bool transferGame( );

        /*
         * @brief: Draws a splash screen with the game's name etc.
         */
        void drawSplash( );

      public:
        void run( );
    };
} // namespace SAVE
