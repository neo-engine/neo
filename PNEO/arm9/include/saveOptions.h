/*
Pokémon neo
------------------------------

file        : options.h
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2021
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

namespace SAVE {
    enum language { EN, GER };

    struct saveOptions {
        u8 m_EXPShareEnabled;
        u8 m_enableBGM : 1  = true;
        u8 m_enableSFX : 1  = true;
        u8 m_difficulty : 6 = 3;
        u8 m_bgIdx;

        language m_language;
        s8       m_levelModifier;
        s8       m_encounterRateModifier;
        s8       m_textSpeedModifier;

        constexpr void clear( ) {
            m_EXPShareEnabled = false;
            m_enableBGM = true;
            m_enableSFX = true;
            m_bgIdx  = 0;
            setDifficulty( 3 );
            setTextSpeed( 1 );
        }

        constexpr u8 getTextSpeed( ) const {
            if( m_textSpeedModifier == -20 ) { return 0; }
            if( m_textSpeedModifier == 0 ) { return 1; }
            return 2;
        }

        constexpr void setTextSpeed( u8 p_value ) {
            if( p_value == 0 ) {
                m_textSpeedModifier = -20;
            } else if( p_value == 1 ) {
                m_textSpeedModifier = 0;
            } else {
                m_textSpeedModifier = 20;
            }
        }

        constexpr void setDifficulty( u8 p_value ) {
            m_difficulty            = p_value;
            m_encounterRateModifier = ( s8( p_value ) - 3 ) * -40;
            m_levelModifier         = ( s8( p_value ) - 3 ) * 5;
        }

        constexpr u8 getDifficulty( ) const {
            return m_difficulty;
        }
    };

    void runSettings( );
} // namespace SAVE
