/*
Pokémon Emerald 2 Version
------------------------------

file        : map2devents.h
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
#include <nds/ndstypes.h>
#include <map>
#include <vector>

#include "map2d.h"

namespace map2d {
    enum MoveMode {
        WALK,
        SURF,
        BIKE
    };
    enum FaceDirection {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    struct map2devent {
        Map::pos m_position;
        u8       m_eventType;
    };

    struct map2dperson : public map2devent {
        u8      m_id;
        u16     m_spriteIdx;
        u8      m_moveType;
        u8      m_movement;
        u8      m_isTrainer;
        u16     m_range;
        u16     m_scriptId;
    };

    struct map2dwarp : public map2devent {
        u8      m_targetWarp;
        char    m_targetMap[ 100 ];
    };

    struct map2dscript : public map2devent {
        u16     m_activationCheckId;
        u16     m_activatedValue;
        u16     m_scriptId;
    };

    struct map2dsign : public map2devent {
        u8      m_signType;
        u16     m_scriptId; //Also itemId
        u8      m_itemTakenId;
        u8      m_itemCount; //Also SecretBaseId
    };

    class map2devents {
    public:
        u8      m_personCnt;
        u8      m_warpCnt;
        u8      m_scriptCnt;
        u8      m_signCnt;

        Map*    m_map;

        std::map<Map::pos, std::vector<map2devent> > m_events;

        map2devents( Map* p_map, const char* p_path, const char* p_name );

        void    triggerStepOn( Map::pos p_pos, MoveMode p_moveMode );
        void    triggerInteract( Map::pos p_pos, FaceDirection p_faceDirection );
    };
}