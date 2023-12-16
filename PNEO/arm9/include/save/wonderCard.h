/*
    Pokémon neo
    ------------------------------

    file        : wonderCard.h
    author      : Philip Wellnitz
    description : Data structure to store gifts distributed via the internet.

    Copyright (C) 2023 - 2023
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
    enum wcEventType : u8 {
        WCTYPE_NONE = 0,
        WCTYPE_ITEM = 1,
        WCTYPE_PKMN = 2,
        // WCTYPE_TRAINER_HILL = 3,
        // WCTYPE_RENTAL_TEAM  = 4,
    };

    struct wonderCard {
        u8          m_unused;
        u8          m_targetGameVersion;
        wcEventType m_type;
        u8          m_id;

        u8 m_data[ 200 ];

        /*
        union data {
            struct {
                u16 m_itemId[ 3 ];
                u16 m_itemCount[ 3 ];
            } m_item;

            struct {
                boxPokemon m_pkmn;
            } m_pkmn;
        } m_data;
        */

        inline wonderCard( ) : m_type{ WCTYPE_NONE } {
        }
    };
} // namespace SAVE
