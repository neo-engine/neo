/*
    Pokémon neo
    ------------------------------

    file        : item.h
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

#include <map>
#include <string>
#include <nds/ndstypes.h>

#include "ability.h"
#include "script.h"
extern const char ITEM_PATH[];

class item {
  public:
    enum itemEffectType {
        NONE          = 0,
        IN_BATTLE     = 1, // Medicine/Berries
        HOLD          = 2, // Has a hold effect only
        OUT_OF_BATTLE = 4, // Repel, etc
        USE_ON_PKMN   = 8  // Evolutionaries,
    };
    enum itemType { GOODS, KEY_ITEM, TM_HM, MAILS, MEDICINE, BERRIES, POKE_BALLS, BATTLE_ITEM };

    struct itemData {
        itemEffectType m_itemEffectType;
        u32            m_price;
        u32            m_itemEffect;

        char m_itemDisplayName[ 15 ];
        char m_itemDescription[ 200 ];
        char m_itemShortDescr[ 100 ];
    } m_itemData;
    bool m_loaded; // Specifies whether the item data has been loaded

    itemType m_itemType;

    std::string m_itemName;

    ability::abilityType m_inBattleEffect;
    BATTLE::battleScript m_inBattleScript;

    // Functions
    std::string getDisplayName( bool p_new = false );

    std::string getDescription( );

    std::string getShortDescription( );

    u32 getEffect( );

    itemEffectType getEffectType( );

    u32 getPrice( );

    u16 getItemId( );

    virtual bool load( );

    bool needsInformation( u8 p_num );

    bool use( pokemon& p_pokemon );

    bool use( bool p_dryRun = false );

    bool useable( );

    // Constructors

    item( const std::string& p_itemName ) : m_itemName( p_itemName ) {
        m_loaded = false;
    }

    item( ) : m_itemName( "Null" ) { /*load = false;*/
    }
};

class ball : public item {
  public:
    ball( const std::string& p_name ) : item( p_name ) {
        m_itemType = POKE_BALLS;
    }
};

class medicine : public item {
  public:
    medicine( const std::string& p_name ) : item( p_name ) {
        m_itemType = MEDICINE;
    }
};

class TM : public item {
  public:
    u16 m_moveIdx;
    TM( const std::string& p_name, u16 p_moveIdx ) : item( p_name ) {
        m_itemType = TM_HM;
        m_moveIdx  = p_moveIdx;
    }
};

class battleItem : public item {
  public:
    battleItem( const std::string& p_name ) : item( p_name ) {
        m_itemType = BATTLE_ITEM;
    }
};

class keyItem : public item {
  public:
    keyItem( const std::string& p_name ) : item( p_name ) {
        m_itemType = KEY_ITEM;
    }
};

class mail : public item {
  public:
    mail( const std::string& p_name ) : item( p_name ) {
        m_itemType = MAILS;
    }
};

#define MAX_ITEMS 1280
extern item* ItemList[ MAX_ITEMS ];
