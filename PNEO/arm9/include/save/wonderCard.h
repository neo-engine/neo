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
        // WCTYPE_POKEBLOCK = 5,
    };

    struct wonderCard {
        u8          m_descriptionId; // 2 * id + 0: before collection, 2 * id + 1: after
        u8          m_targetGameVersion;
        wcEventType m_type;
        u8          m_id;

        u8 m_year;
        u8 m_month;
        u8 m_day;
        u8 m_reserved;

        char m_title[ 96 ];

        union data {
            struct {
                u16 m_itemId[ 3 ];
                u16 m_itemCount[ 3 ];
            } m_item;
            struct {
                u16  m_moves[ 4 ];
                u16  m_species;
                char m_name[ PKMN_NAMELENGTH ]; // m_name[0] is 0 for default name
                u16  m_level;
                u16  m_id;
                u16  m_sid;
                char m_oT[ OTLENGTH ]; // m_oT[0] is 0 if player is OT
                u8   m_shiny;
                bool m_hiddenAbility;
                bool m_fatefulEncounter;
                bool m_isEgg;
                u16  m_gotPlace;
                u8   m_female;
                u8   m_ball;
                u8   m_pokerus;
                u8   m_forme;
                u8   m_ribbons[ 12 ];
                u16  m_items[ 4 ];
            } m_pkmn;
            u8 m_data[ 100 ];
        } m_data;

        inline wonderCard( ) : m_type{ WCTYPE_NONE } {
        }

        inline wonderCard( u8 p_descriptionId, u8 p_cardId, const char* p_title, u16 p_itemId1,
                           u8 p_itemCount1 = 1, u16 p_itemId2 = 0, u8 p_itemCount2 = 0,
                           u16 p_itemId3 = 0, u8 p_itemCount3 = 0 )
            : m_descriptionId{ p_descriptionId }, m_type{ WCTYPE_ITEM }, m_id{ p_cardId } {
            memcpy( m_title, p_title, 96 );
            m_data.m_item.m_itemId[ 0 ] = p_itemId1;
            m_data.m_item.m_itemId[ 1 ] = p_itemId2;
            m_data.m_item.m_itemId[ 2 ] = p_itemId3;

            m_data.m_item.m_itemCount[ 0 ] = p_itemCount1;
            m_data.m_item.m_itemCount[ 1 ] = p_itemCount2;
            m_data.m_item.m_itemCount[ 2 ] = p_itemCount3;
        }

        inline wonderCard( u8 p_descriptionId, u8 p_cardId, const char* p_title,
                           const u16 p_moves[ 4 ], u16 p_pkmnId, const char* p_name, u16 p_level,
                           u16 p_id, u16 p_sid, const char* p_ot, u8 p_shiny, bool p_hiddenAbility,
                           bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace, u8 p_female,
                           u8 p_ball, u8 p_pokerus, u8 p_forme, const u8 p_ribbons[ 12 ],
                           const u16 p_items[ 4 ] )
            : m_descriptionId{ p_descriptionId }, m_type{ WCTYPE_PKMN }, m_id{ p_cardId } {
            memcpy( m_title, p_title, 96 );
            memcpy( m_data.m_pkmn.m_moves, p_moves, 4 );
            if( p_name ) { memcpy( m_data.m_pkmn.m_name, p_name, PKMN_NAMELENGTH ); }
            if( p_ot ) { memcpy( m_data.m_pkmn.m_oT, p_ot, OTLENGTH ); }
            if( p_ribbons ) { memcpy( m_data.m_pkmn.m_ribbons, p_ribbons, 12 ); }
            if( p_items ) { memcpy( m_data.m_pkmn.m_items, p_items, 8 ); }

            m_data.m_pkmn.m_species          = p_pkmnId;
            m_data.m_pkmn.m_level            = p_level;
            m_data.m_pkmn.m_id               = p_id;
            m_data.m_pkmn.m_sid              = p_sid;
            m_data.m_pkmn.m_shiny            = p_shiny;
            m_data.m_pkmn.m_hiddenAbility    = p_hiddenAbility;
            m_data.m_pkmn.m_fatefulEncounter = p_fatefulEncounter;
            m_data.m_pkmn.m_isEgg            = p_isEgg;
            m_data.m_pkmn.m_gotPlace         = p_gotPlace;
            m_data.m_pkmn.m_female           = p_female;
            m_data.m_pkmn.m_ball             = p_ball;
            m_data.m_pkmn.m_pokerus          = p_pokerus;
            m_data.m_pkmn.m_forme            = p_forme;
        }
    };
} // namespace SAVE
