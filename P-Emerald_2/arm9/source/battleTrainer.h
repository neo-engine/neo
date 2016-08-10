/*
Pokémon Emerald 2 Version
------------------------------

file        : battleTrainer.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2016
Philip Wellnitz

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
#include <nds/ndstypes.h>
#include <string>
#include <vector>
#include "defines.h"

struct pokemon;

namespace BATTLE {
    extern std::string trainerClassNames[ 120 ];

    class battleTrainer {
    public:
        enum trainerClass {
            PKMN_TRAINER = 0, //Wally

        };

        std::string         m_battleTrainerName;
        trainerClass        m_trainerClass;
        std::vector<pokemon>& m_pkmnTeam;
        std::pair<u16, u16>*m_items;         //if this is 0 and _itemCount == MAX_ITEMS_IN_BAG, then FS::SAV->m_bag is used.
        u16                 m_itemCount;
    private:
        int                 _moneyEarned;
        std::string         _msg1;
        std::string         _msg2;
        std::string         _msg3;
        std::string         _msg4;

    public:
        battleTrainer( std::string p_battleTrainerName,
                       std::string p_msg1,
                       std::string p_msg2,
                       std::string p_msg3,
                       std::string p_msg4,
                       std::vector<pokemon>& p_pkmnTeam,
                       std::pair<u16, u16>* p_items = 0,
                       u16 p_itemCnt = MAX_ITEMS_IN_BAG,
                       trainerClass p_trainerClass = PKMN_TRAINER )
            : m_battleTrainerName( p_battleTrainerName ),
            m_trainerClass( p_trainerClass ),
            m_pkmnTeam( p_pkmnTeam ),
            m_items( p_items ),
            m_itemCount( p_itemCnt ),
            _msg1( p_msg1 ),
            _msg2( p_msg2 ),
            _msg3( p_msg3 ),
            _msg4( p_msg4 ) { }

        const char*         getLooseMsg( ) const {
            return _msg4.c_str( );
        }
        int                 getLooseMoney( ) const {
            return _moneyEarned;
        }
        const char*         getWinMsg( ) const {
            return _msg3.c_str( );
        }
        const char*         getCriticalMsg( ) const {
            return _msg2.c_str( );
        }
        const char*         getInitMsg( ) const {
            return _msg1.c_str( );
        }
    };
}
