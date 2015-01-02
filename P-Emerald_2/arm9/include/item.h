/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : item.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2015
    Philip Wellnitz (RedArceus)

    This file is part of Pokémon Emerald 2 Version.

    Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
    */

#pragma once

#include <string>
#include <map>

#include "ability.h"

#include <nds/ndstypes.h>

extern const char ITEM_PATH[ ];
class ability;

namespace ITEMS {

    class item {
    public:
        enum itemEffectType {
            NONE = 0,
            IN_BATTLE = 1,
            HOLD = 2,
            OUT_OF_BATTLE = 4,
            USE_ON_PKMN = 8
        };
        enum itemType {
            GOODS,
            KEY_ITEM,
            TM_HM,
            MAILS,
            MEDICINE,
            BERRIES,
            POKE_BALLS,
            BATTLE_ITEM
        };

        std::string     m_itemName;//, displayName, dscrpt;
        //EFFEKT effekt;
        itemType        m_itemType;
        ability::abilityType
            m_inBattleEffect;
        BATTLE::battleScript
            m_inBattleScript;

        //std::string effekt_script;
        //int price;

        bool            m_loaded; //Specifies whether the item data has been loaded

        //Functions
        std::string     getDisplayName( bool p_new = false );

        std::string     getDescription( bool p_new = false );

        std::string     getShortDescription( bool p_new = false );

        itemEffectType  getEffectType( );

        itemType        getItemType( );

        u32             getPrice( );

        u16             getItemId( );

        virtual bool    load( );

        void            use( ... );

        //Constructors

        item( const std::string& p_itemName )
            : m_itemName( p_itemName ) {/* load = false;*/
        }

        item( )
            : m_itemName( "Null" ) { /*load = false;*/
        }

    };

    class ball
        : public item {
    public:
        ball( const std::string& p_name )
            : item( p_name ) {
            this->m_itemType = POKE_BALLS;
        }
    };

    class medicine
        : public item {
    public:
        medicine( const std::string& p_name )
            : item( p_name ) {
            this->m_itemType = MEDICINE;
        }
    };

    class TM
        : public item {
    public:
        TM( const std::string& p_name )
            : item( p_name ) {
            this->m_itemType = TM_HM;
        }
    };

    class battleItem
        : public item {
    public:
        battleItem( const std::string& p_name )
            : item( p_name ) {
            this->m_itemType = BATTLE_ITEM;
        }
    };

    class keyItem
        : public item {
    public:
        keyItem( const std::string& p_name )
            : item( p_name ) {
            this->m_itemType = KEY_ITEM;
        }
    };

    class mail
        : public item {
    public:
        mail( const std::string& p_name )
            : item( p_name ) {
            this->m_itemType = MAILS;
        }
    };


    extern item ItemList[ 700 ];
}

#define I_LUCKY_EGG 42
#define I_MACHO_BRACE 43
#define I_POWER_WEIGHT 44
#define I_POWER_BRACER 45
#define I_POWER_BELT 46
#define I_POWER_LENS 47
#define I_POWER_BAND 48
#define I_POWER_ANKLET 49
#define I_LAGGING_TAIL 50
#define I_QUICK_CLAW 51
#define I_FULL_INCENSE 52