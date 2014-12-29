/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : item.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
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